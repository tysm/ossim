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
    explicit MemoryManager(unsigned shift_delay, size_t ram_pages,
                           size_t virtual_pages) :
        shift_delay(shift_delay), next_virtual_position(0),
        page_table(virtual_pages, {-1, false}), ram(ram_pages, 0)
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

private:
    bool alloc(unsigned pid, size_t &ref, bool blocked_process);

    virtual void update_access_table(size_t alloc_position)
    {
    }

    virtual auto next_alloc_position() -> size_t = 0;

    virtual void regress_alloc_position()
    {
    }

    std::shared_ptr<std::vector<size_t> > refs_in_use;
    unsigned shift_delay;
    unsigned delay;

    size_t next_virtual_position;
    std::vector<std::pair<int, bool> > page_table;
    std::multiset<unsigned> swap;

    std::unique_ptr<Process> process;
    std::list<size_t*> alloc_buffer;

protected:
    std::vector<unsigned> ram;
};

class FIFO_MM : public MemoryManager
{
public:
    explicit FIFO_MM(unsigned shift_delay, size_t ram_pages,
                     size_t virtual_pages) :
        MemoryManager(shift_delay, ram_pages, virtual_pages),
        alloc_position(0)
    {
    }

private:
    auto next_alloc_position() -> size_t override
    {
        alloc_position %= ram.size();
        return alloc_position++;
    }

    void regress_alloc_position() override
    {
        alloc_position--;
        alloc_position = min(alloc_position, ram.size()-1);
    }

    size_t alloc_position;
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
    explicit LRU(unsigned shift_delay, size_t ram_pages,
                 size_t virtual_pages) :
        MemoryManager(shift_delay, ram_pages, virtual_pages),
        access_table(ram_pages, 0), current_access(1)
    {
        for(auto it = access_table.begin(); it != access_table.end(); ++it)
        {
            alloc_position.push(it);
        }
    }

private:
    void update_access_table(size_t alloc_position) override
    {
        access_table[alloc_position] = current_access++;

        // Forces reorganization of the priority_queue
        auto it = this->alloc_position.top();
        this->alloc_position.pop();
        this->alloc_position.push(it);
    }

    auto next_alloc_position() -> size_t override
    {
        return alloc_position.top() - access_table.begin();
    }

    std::vector<unsigned> access_table;

    unsigned current_access;

    std::priority_queue<std::vector<unsigned>::iterator,
                        std::vector<std::vector<unsigned>::iterator>,
                        LRUComparator> alloc_position;
};
}
