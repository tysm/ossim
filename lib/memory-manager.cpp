#include <sosim/memory-manager.hpp>

namespace sosim
{
auto MemoryManager::check(std::shared_ptr<Process> process) -> std::shared_ptr<Process>
{
    for(auto ref : process->page_refs)
    {
        // Unallocated process
        if(ref == -1)
            return alloc(std::move(process));

        // Page fault
        else if(!page_table[ref].second)
        {
            bloqued.push_back(std::move(process));
            return nullptr;
        }
    }
    return process;
}

auto MemoryManager::alloc(std::shared_ptr<Process> process) -> std::shared_ptr<Process>
{
    for(auto ref : process->page_refs)
    {
        // Unallocated page
        if(ref == -1)
        {
            // There is nothing allocated
            if(!ram[alloc_position])
            {
                ram[alloc_position] = process->pid;
                page_table[ref] = {alloc_position, true};
                update_alloc_position();
            }
            else
            {
                bloqued.push_back(std::move(process));
                return nullptr;
            }
        }
    }
    return process;
}
}
