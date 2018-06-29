#pragma once
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
                           std::list<std::shared_ptr<Process> > &blocked) :
        shift_delay(shift_delay), ram(ram_pages, 0), swap(),
        alloc_position(0), page_table(virtual_pages, {-1, false}),
        blocked(blocked)
    {
    }

    auto run() -> std::shared_ptr<Process>;

    auto check(std::shared_ptr<Process> process) -> std::shared_ptr<Process>;

private:
    auto alloc(std::shared_ptr<Process> process) -> std::shared_ptr<Process>;
    
    virtual void update_alloc_position();

    unsigned shift_delay;

    std::vector<unsigned> ram;
    std::vector<unsigned> swap;

    int alloc_position;

    std::vector<std::pair<int, bool> > page_table;

    std::list<std::shared_ptr<Process> > &blocked;
};
}
