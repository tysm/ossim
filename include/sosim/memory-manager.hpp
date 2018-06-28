#pragma once
#include <list>
#include <memory>
#include <utility>
#include <sosim/commom.hpp>

namespace sosim
{
class MemoryManager
{
public:
    explicit MemoryManager(const unsigned shift_delay, const int ram_pages,
                           const int virtual_pages) :
        shift_delay(shift_delay), ram(ram_pages, 0), swap(),
        alloc_position(0), page_table(virtual_pages, {-1, false}), bloqued()
    {
    }

    auto run() -> std::shared_ptr<Process>;

    auto check(std::shared_ptr<Process> process) -> std::shared_ptr<Process>;

private:
    auto alloc(std::shared_ptr<Process> process) -> std::shared_ptr<Process>;
    
    virtual void update_alloc_position();

    unsigned shift_delay;

    std::list<unsigned> ram;
    std::list<unsigned> swap;

    int alloc_position;

    std::list<pair<int, bool> > page_table;

    std::list<std::shared_ptr<Process> > bloqued;
};
}
