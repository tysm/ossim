#include <sosim/memory-manager.hpp>

namespace sosim
{
auto MemoryManager::run() -> std::unique_ptr<Process>
{
    if(!blocked.empty())
    {
        if(!delay_count)
            delay_count = shift_delay;

        unsigned pid = blocked.front()->pid;
        size_t &ref = *alloc_buffer.front().front();

        if(ref == size_t(-1))
        {
            if(virtual_position != page_table.size())
            {
                ref = virtual_position;
                virtual_position++;
            }
            // TODO else crash
        }
        else
            swap.erase(pid);

        swap.insert(ram[alloc_position]);

        ram[alloc_position] = pid;
        page_table[ref] = {alloc_position, true};
        update_alloc_position();

        delay_count--;
        if(!delay_count)
        {
            auto process = std::move(blocked.front());
            blocked.pop_front();
            return process;
        }
    }
    return nullptr;
}

auto MemoryManager::check(std::unique_ptr<Process> process)
        -> std::unique_ptr<Process>
{
    bool block = false;
    for(size_t i = 0; i < process->page_refs.size(); ++i)
    {
        size_t &ref = process->page_refs[i];
        if(!block)
        {
            // Unallocated process
            if(ref == size_t(-1))
                return alloc(std::move(process), i);

            // Page fault
            else if(!page_table[ref].second)
            {
                block = true;
                alloc_buffer.push_back(std::list<size_t*>());
                alloc_buffer.back().push_back(&ref);
            }
        }
        else
        {
            alloc_buffer.back().push_back(&ref);
        }
    }
    if(block)
        blocked.push_back(std::move(process));
    return process;
}

auto MemoryManager::alloc(std::unique_ptr<Process> process, size_t first_idx)
        -> std::unique_ptr<Process>
{
    bool block = false;
    for(size_t i = first_idx; i < process->page_refs.size(); ++i)
    {
        size_t &ref = process->page_refs[i];
        if(!block)
        {
            // There is nothing allocated
            if(!ram[alloc_position])
            {
                if(virtual_position != page_table.size())
                {
                    ref = virtual_position;
                    virtual_position++;

                    ram[alloc_position] = process->pid;
                    page_table[ref] = {alloc_position, true};
                    update_alloc_position();
                }
                // TODO else crash!
            }
            else
            {
                block = true;
                alloc_buffer.push_back(std::list<size_t*>());
                alloc_buffer.back().push_back(&ref);
            }
        }
        else
        {
            alloc_buffer.back().push_back(&ref);
        }
    }
    if(block)
        blocked.push_back(std::move(process));
    return process;
}
}
