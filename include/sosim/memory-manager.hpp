#pragma once
#include <set>
#include <list>
#include <vector>
#include <memory>
#include <utility>
#include <sosim/commom.hpp>

namespace sosim
{
class MemoryManager
{
public:
    explicit MemoryManager(unsigned shift_delay, int ram_pages,
                           int virtual_pages,
                           std::list<std::unique_ptr<Process> > &blocked) :
        shift_delay(shift_delay), delay_count(0), ram(ram_pages, 0), swap(),
        alloc_position(0), virtual_position(0),
        page_table(virtual_pages, {-1, false}),
        blocked(blocked), alloc_buffer()
    {
    }

    auto run() -> std::unique_ptr<Process>;

    auto check(std::unique_ptr<Process> process) -> std::unique_ptr<Process>;

private:
    auto alloc(std::unique_ptr<Process> process, size_t first_idx)
            -> std::unique_ptr<Process>;
    
    virtual void update_alloc_position();

    unsigned shift_delay;
    unsigned delay_count;

    std::vector<unsigned> ram;
    std::multiset<unsigned> swap;

    size_t alloc_position;
    size_t virtual_position;

    std::vector<std::pair<int, bool> > page_table;

    std::list<std::unique_ptr<Process> > &blocked;
    std::list<std::list<size_t*> > alloc_buffer;
};
}
