#include <sosim/memory-manager.hpp>

namespace sosim
{
auto MemoryManager::run() -> std::unique_ptr<Process>
{
    if(process)
    {
        delay--;

        if(delay%shift_delay == 0)
        {
            alloc(process->pid, *alloc_buffer.front(), true);
            alloc_buffer.pop_front();
        }

        if(!delay)
            return std::move(process);
    }
    return nullptr;
}

void MemoryManager::push(std::unique_ptr<Process> process)
{
    for(size_t i = 0; i < process->page_refs->size(); ++i)
    {
        auto &ref = (*process->page_refs)[i];
        // Unallocated process or page fault
        if(ref == size_t(-1) || !page_table[ref].second)
            alloc_buffer.push_back(&ref);
    }
    delay = shift_delay*alloc_buffer.size();
    this->process = std::move(process);
}

bool MemoryManager::try_alloc(unsigned pid,
                              std::shared_ptr<std::vector<size_t> > page_refs,
                              bool checking)
{
    for(size_t i = 0; i < page_refs->size(); ++i)
    {
        auto &ref = (*page_refs)[i];
        // Unallocated process
        if(ref == size_t(-1) && !alloc(pid, ref, false))
            return false;

        // Page fault
        else if(!page_table[ref].second)
            return false;

        // Re-access successfully
        else if(!checking)
            update_access_table(page_table[ref].first);
    }
    refs_in_use = std::move(page_refs);
    return true;
}

bool MemoryManager::alloc(unsigned pid, size_t &ref, bool blocked_process)
{
    auto alloc_position = next_alloc_position();
    // Unallocated process
    if(ref == size_t(-1))
    {
        if(next_virtual_position != page_table.size())
            ref = next_virtual_position++;
        // TODO else crash

        // There's not empty space, can't alloc
        if(!blocked_process && ram[alloc_position] != 0)
        {
            regress_alloc_position();
            return false;
        }
    }

    // Process allocated but your page is in swap
    else if(blocked_process)
        swap.erase(pid);

    // Allocating in ram
    if(ram[alloc_position] != 0)
        swap.insert(ram[alloc_position]);
    ram[alloc_position] = pid;
    page_table[ref] = {alloc_position, true};

    // New access successfully
    update_access_table(alloc_position);
    return true;
}
}
