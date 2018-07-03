#pragma once
#include <set>
#include <list>
#include <queue>
#include <vector>
#include <memory>
#include <utility>
#include <sosim/commom.hpp>

namespace sosim
{
template <class T>
const T& min(const T &a, const T &b)
{
    return !(b < a)? a : b;
}

enum class MemoryManagerState
{
    Busy,
    Free,
};

enum class MemoryManagerKind
{
    FIFO_MM,
    LRU,
};

class MemoryManager
{
public:
    explicit MemoryManager(unsigned shift_delay, size_t virtual_pages,
                           size_t ram_pages) :
        shift_delay(shift_delay), page_table(virtual_pages, {-1, false}),
        ram(ram_pages, 0)
    {
    }

    auto run() -> std::unique_ptr<Process>;

    void push(std::unique_ptr<Process> process);

    bool try_alloc(unsigned pid,
                   std::shared_ptr<std::vector<size_t> > page_refs,
                   bool checking);

    auto state() -> MemoryManagerState
    {
        return process? MemoryManagerState::Busy : MemoryManagerState::Free;
    }

    void dealloc()
    {
        if(refs_in_use)
        {
            for(auto ref : *refs_in_use)
            {
                page_table[ref] = {-1, false};
            }
            refs_in_use.reset();
        }
    }

private:
    bool alloc(unsigned pid, size_t &ref, bool blocked_process);

    virtual void make_updates(size_t virtual_position, size_t alloc_position)
    {
    }

    auto was_allocated(size_t alloc_position) -> size_t
    {
        for(size_t i = 0; i < page_table.size(); ++i)
        {
            bool used = alloc_position == page_table[i].first;
            if(used && page_table[i].second)
                return i;
        }
        return -1;
    }

    virtual auto virtual_position() -> size_t = 0;

    virtual auto alloc_position() -> size_t = 0;

    virtual void regress_virtual_position()
    {
    }

    virtual void regress_alloc_position()
    {
    }

    std::shared_ptr<std::vector<size_t> > refs_in_use;
    unsigned shift_delay;
    unsigned delay;

    std::multiset<unsigned> swap;

    std::unique_ptr<Process> process;
    std::list<size_t*> alloc_buffer;

protected:
    bool valid_virtual_position(size_t virtual_position)
    {
        return page_table[virtual_position].first == size_t(-1);
    }

    bool valid_alloc_position(size_t alloc_position)
    {
        if(refs_in_use)
        {
            for(auto ref : *refs_in_use)
            {
                if(ref == alloc_position)
                    return false;
            }
        }
        return true;
    }

    std::vector<std::pair<size_t, bool> > page_table;
    std::vector<unsigned> ram;
};

class FIFO_MM : public MemoryManager
{
public:
    explicit FIFO_MM(unsigned shift_delay, size_t virtual_pages,
                     size_t ram_pages) :
        MemoryManager(shift_delay, virtual_pages, ram_pages),
        next_virtual_position(0), next_alloc_position(0)
    {
    }

private:
    auto virtual_position() -> size_t override
    {
        auto p = next_virtual_position %= page_table.size();
        auto last = p;
        while(!valid_virtual_position(p) && ++p != last)
            p %= page_table.size();
        //if(p == last)
            // TODO catch full virtual memory error
        next_virtual_position = p;
        return next_virtual_position++;
    }

    auto alloc_position() -> size_t override
    {
        auto p = next_alloc_position %= ram.size();
        auto last = p;
        while(!valid_alloc_position(p) && ++p != last)
            p %= ram.size();
        //if(p == last)
            // TODO catch full memory error
        next_alloc_position = p;
        return next_alloc_position++;
    }

    void regress_virtual_position() override
    {
        next_virtual_position--;
        next_virtual_position = min(next_virtual_position,
                                    page_table.size() - 1);
    }

    void regress_alloc_position() override
    {
        next_alloc_position--;
        next_alloc_position = min(next_alloc_position, ram.size() - 1);
    }

    size_t next_virtual_position;
    size_t next_alloc_position;
};

class LRUComparator
{
public:
    bool operator() (const std::vector<unsigned>::iterator &i,
                     const std::vector<unsigned>::iterator &j)
    {
        return *i >= *j;
    }
};

class LRU : public MemoryManager
{
public:
    explicit LRU(unsigned shift_delay, size_t virtual_pages,
                 size_t ram_pages) :
        MemoryManager(shift_delay, virtual_pages, ram_pages),
        virtual_access_table(virtual_pages, 0), ram_access_table(ram_pages, 0),
        current_access(1)
    {
        for(auto it = virtual_access_table.begin();
            it != virtual_access_table.end(); ++it)
        {
            next_virtual_position.push(it);
        }

        for(auto it = ram_access_table.begin();
            it != ram_access_table.end(); ++it)
        {
            next_alloc_position.push(it);
        }
    }

private:
    void make_updates(size_t virtual_position, size_t alloc_position) override
    {
        virtual_access_table[virtual_position] = current_access;
        ram_access_table[alloc_position] = current_access++;

        // Forces reorganization of the priority_queue
        auto virtual_it = this->next_virtual_position.top();
        this->next_virtual_position.pop();
        this->next_virtual_position.push(virtual_it);

        auto alloc_it = this->next_alloc_position.top();
        this->next_alloc_position.pop();
        this->next_alloc_position.push(alloc_it);
    }

    auto virtual_position() -> size_t override
    {
        size_t p = next_virtual_position.top() - virtual_access_table.begin();
        //if(!valid_virtual_position(p))
            // TODO catch full virtual memory error
        return p;
    }

    auto alloc_position() -> size_t override
    {
        size_t p = next_alloc_position.top() - ram_access_table.begin();
        //if(!valid_alloc_position(p))
            // TODO catch full memory error
        return p;
    }

    std::vector<unsigned> virtual_access_table;
    std::vector<unsigned> ram_access_table;

    unsigned current_access;

    std::priority_queue<std::vector<unsigned>::iterator,
                        std::vector<std::vector<unsigned>::iterator>,
                        LRUComparator> next_virtual_position;
    std::priority_queue<std::vector<unsigned>::iterator,
                        std::vector<std::vector<unsigned>::iterator>,
                        LRUComparator> next_alloc_position;
};
}
