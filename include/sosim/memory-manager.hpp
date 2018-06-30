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

class LRUComparator
{
public:
    bool operator() (const std::vector<unsigned>::iterator &i,
                     const std::vector<unsigned>::iterator &j)
    {
        return *i < *j;
    }
};

class MemoryManager
{
public:
    explicit MemoryManager(unsigned shift_delay, size_t ram_pages,
                           size_t virtual_pages,
                           std::list<std::unique_ptr<Process> > &blocked) :
        shift_delay(shift_delay), delay_count(0), swap(), virtual_position(0),
        page_table(virtual_pages, {-1, false}), blocked(blocked),
        alloc_buffer(), ram(ram_pages, 0)
    {
    }

    auto run() -> std::unique_ptr<Process>;

    auto check(std::unique_ptr<Process> process) -> std::unique_ptr<Process>;

private:
    auto alloc(std::unique_ptr<Process> process, size_t first_idx)
            -> std::unique_ptr<Process>;

    virtual void update_access_table(size_t ref) = 0;

    virtual auto next_alloc_position() -> size_t = 0;

    virtual void regress_alloc_position() = 0;

    unsigned shift_delay;
    unsigned delay_count;

    std::multiset<unsigned> swap;

    size_t virtual_position;

    std::vector<std::pair<int, bool> > page_table;

    std::list<std::unique_ptr<Process> > &blocked;
    std::list<std::list<size_t*> > alloc_buffer;

protected:
    std::vector<unsigned> ram;
};

class FIFO_MM : public MemoryManager
{
public:
    explicit FIFO_MM(unsigned shift_delay, int ram_pages, int virtual_pages,
                     std::list<std::unique_ptr<Process> > &blocked) :
        MemoryManager(shift_delay, ram_pages, virtual_pages, blocked),
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

class LRU : public MemoryManager
{
public:
    explicit LRU(unsigned shift_delay, int ram_pages, int virtual_pages,
                 std::list<std::unique_ptr<Process> > &blocked) :
        MemoryManager(shift_delay, ram_pages, virtual_pages, blocked),
        access_table(ram_pages, 0), current_access(1), alloc_position()
    {
        for(std::vector<unsigned>::iterator it = access_table.begin();
            it != access_table.end(); ++it)
        {
            alloc_position.push(it);
        }
    }
private:
    void update_access_table(size_t ref) override
    {
        access_table[ref] = current_access++;
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
