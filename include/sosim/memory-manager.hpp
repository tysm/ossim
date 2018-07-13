#pragma once
#include <list>
#include <queue>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <sosim/commom.hpp>

namespace sosim
{
/// The possible states a memory manager.
enum class MemoryManagerState
{
    Busy,
    Free,
};

/// The subclass of a memory manager.
enum class MemoryManagerKind
{
    FIFO_MM,
    LRU,
};

/// Base of any memory manager.
class MemoryManager
{
public:
    explicit MemoryManager(unsigned shift_delay, size_t virtual_pages,
                           size_t ram_pages) :
        shift_delay(shift_delay), page_table(virtual_pages, {-1, false}),
        ram(ram_pages, {0, -1})
    {
    }

    /// Performs the paging algorithm.
    auto run() -> std::unique_ptr<Process>;

    /// Pushes a process to be paginated.
    void push(std::unique_ptr<Process> process);

    /// Checks if pages referenced by some process with ID 'pid' are in 'ram'.
    /// If the process isn't allocated, it tries to allocate.
    ///
    /// When !'checking' it may influence the paging algorithm.
    bool try_alloc(unsigned pid,
                   std::shared_ptr<vector_pair<size_t, size_t> > page_refs,
                   bool checking);

    /// Returns its current state.
    auto state() -> MemoryManagerState
    {
        return process? MemoryManagerState::Busy : MemoryManagerState::Free;
    }

    /// Flushes the pages references of some process that's no longer in use
    /// holds on 'refs_in_use'.
    ///
    /// Furthermore, when a process ends it should free up the virtual memory.
    /// It happens when !'just_flush'.
    void dealloc(bool just_flush)
    {
        if(refs_in_use)
        {
            if(!just_flush)
            {
                for(auto ref : *refs_in_use)
                    page_table[ref.first] = {-1, false};
            }
            refs_in_use.reset();
        }
    }

private:
    /// Tries to allocate a page refered by 'ref' of some process with ID pid.
    ///
    /// When 'force_alloc', it performs paging.
    bool alloc(unsigned pid, std::pair<size_t, size_t> &ref, bool force_alloc);

    /// Makes updates needed by the current memory manager.
    virtual void make_updates(size_t virtual_position, size_t alloc_position)
    {
    }

    /// Checks if there's a valid page allocated on 'alloc_position' in 'ram'.
    ///
    /// If there's returns its index in 'ram'.
    /// Otherwise, returns -1.
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

    /// Returns a valid (free) virtual allocation position in 'page_table'.
    virtual auto virtual_position() -> size_t = 0;

    /// Returns a valid (but not always free) allocation position in 'ram'.
    virtual auto alloc_position() -> size_t = 0;

    /// Undo the last 'alloc_position' call.
    virtual void regress_alloc_position()
    {
    }

    /// Holds the 'page_refs' of the process that is in the CPU.
    std::shared_ptr<vector_pair<size_t, size_t> > refs_in_use;

    /// Paging delay per page to be paginated.
    unsigned shift_delay;
    /// Current delay counter.
    unsigned delay;

    /// Disk space abstraction.
    vector_pair<unsigned, size_t> swap;

    /// Current process on paging.
    std::unique_ptr<Process> process;
    /// Buffer of pages to be allocated in 'ram'.
    std::list<std::pair<size_t, size_t>* > alloc_buffer;

protected:
    /// Checks if 'virtual_position' is a valid (free) virtual allocation
    /// position in 'page_table'.
    bool valid_virtual_position(size_t virtual_position)
    {
        return page_table[virtual_position].first == size_t(-1);
    }

    /// Checks if 'alloc_position' is a valid allocation position in 'ram'.
    ///
    /// P.S.: 'alloc_position' doesn't have to be free to be valid.
    bool valid_alloc_position(size_t alloc_position)
    {
        if(refs_in_use)
        {
            for(auto ref : *refs_in_use)
            {
                if(page_table[ref.first].first == alloc_position)
                    return false;
            }
        }
        return true;
    }

public: // CAGUEI AQUI
    /// Page table (virtual memory) abstraction.
    ///
    /// The first attribute holds a 'ram' address refered by some process
    /// and the second attribute marks its as valid or invalid.
    vector_pair<size_t, bool> page_table;

    /// RAM abstraction.
    ///
    /// A page here is represented by its respective process 'pid' on first
    /// attribute and its respective index in the process 'page_refs' on second
    /// attribute.
    vector_pair<unsigned, size_t> ram;
};

/// First In First Out memory manager.
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
    /// Returns a valid (free) virtual allocation position in 'page_table'.
    auto virtual_position() -> size_t override
    {
        next_virtual_position %= page_table.size();
        auto p = next_virtual_position;
        auto last = p;
        while(!valid_virtual_position(p) && (++p) % page_table.size() != last)
            p %= page_table.size();
        //if(p == last)
            // TODO catch full virtual memory error.
        next_virtual_position = p;
        return next_virtual_position++;
    }

    /// Returns a valid (but not always free) allocation position in 'ram'.
    auto alloc_position() -> size_t override
    {
        auto p = next_alloc_position %= ram.size();
        auto last = p;
        while(!valid_alloc_position(p) && ++p != last)
            p %= ram.size();
        //if(p == last)
            // TODO catch full memory error.
        next_alloc_position = p;
        return next_alloc_position++;
    }

    /// Undo the last 'alloc_position' call.
    void regress_alloc_position() override
    {
        next_alloc_position--;
        next_alloc_position = std::min(next_alloc_position, ram.size() - 1);
    }

    size_t next_virtual_position;
    size_t next_alloc_position;
};

/// Coparator used by the LRU priority_queue.
class LRUComparator
{
public:
    bool operator() (const std::vector<unsigned>::iterator &i,
                     const std::vector<unsigned>::iterator &j)
    {
        return *i >= *j;
    }
};

/// Less Recent Used memory manager.
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

        // Forces reorganization of the priority_queue.
        auto virtual_it = this->next_virtual_position.top();
        this->next_virtual_position.pop();
        this->next_virtual_position.push(virtual_it);

        auto alloc_it = this->next_alloc_position.top();
        this->next_alloc_position.pop();
        this->next_alloc_position.push(alloc_it);
    }

    /// Returns a valid (free) virtual allocation position in 'page_table'.
    auto virtual_position() -> size_t override
    {
        size_t p = next_virtual_position.top() - virtual_access_table.begin();
        //if(!valid_virtual_position(p))
            // TODO catch full virtual memory error.
        return p;
    }

    /// Returns a valid (but not always free) allocation position in 'ram'.
    auto alloc_position() -> size_t override
    {
        size_t p = next_alloc_position.top() - ram_access_table.begin();
        //if(!valid_alloc_position(p))
            // TODO catch full memory error.
        return p;
    }

    std::vector<unsigned> virtual_access_table;
    std::vector<unsigned> ram_access_table;

    /// Access counter.
    unsigned current_access;

    std::priority_queue<std::vector<unsigned>::iterator,
                        std::vector<std::vector<unsigned>::iterator>,
                        LRUComparator> next_virtual_position;
    std::priority_queue<std::vector<unsigned>::iterator,
                        std::vector<std::vector<unsigned>::iterator>,
                        LRUComparator> next_alloc_position;
};
}
