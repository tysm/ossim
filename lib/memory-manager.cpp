#include <sosim/memory-manager.hpp>

namespace sosim
{
auto MemoryManager::run() -> std::unique_ptr<Process>
{
    if(process)
    {
        while(delay != 1 && (*alloc_buffer.front()).first == size_t(-1) &&
              alloc(process->pid, *alloc_buffer.front(), false))
        {
            delay--;
            alloc_buffer.pop_front();
        }

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
        if(ref.first == size_t(-1) || !page_table[ref.first].second)
            alloc_buffer.push_back(&ref);
    }
    delay = shift_delay*alloc_buffer.size();
    this->process = std::move(process);
}

bool MemoryManager::try_alloc(unsigned pid,
                              std::shared_ptr<vector_pair<size_t,
                                                          size_t> > page_refs,
                              bool checking)
{
    for(size_t i = 0; i < page_refs->size(); ++i)
    {
        auto &ref = (*page_refs)[i];
        // Unallocated process
        if(ref.first == size_t(-1) && !alloc(pid, ref, false))
            return false;

        // Page fault
        else if(!page_table[ref.first].second)
            return false;

        // Re-access successfully
        else if(!checking)
            make_updates(ref.first, page_table[ref.first].first);
    }
    refs_in_use = std::move(page_refs);
    return true;
}

bool MemoryManager::alloc(unsigned pid, std::pair<size_t, size_t> &ref,
                          bool force_alloc)
{
    auto alloc_position = this->alloc_position();
    auto used_ref = was_allocated(alloc_position);
    // Unallocated process
    if(ref.first == size_t(-1))
    {
        // There's not empty space, can't alloc
        if(!force_alloc && used_ref != size_t(-1))
        {
            regress_alloc_position();
            return false;
        }
        ref.first = virtual_position();
    }

    // Process allocated but its page is in swap
    else
    {
        auto it = swap.begin();
        while((*it).first != pid || (*it).second != ref.second)
            it++;
        swap.erase(it);
    }

    // Allocating in ram
    if(used_ref != size_t(-1))
    {
        page_table[used_ref].second = false;
        swap.push_back(ram[alloc_position]);
    }
    ram[alloc_position] = {pid, ref.second};
    page_table[ref.first] = {alloc_position, true};

    // New access successfully
    make_updates(ref.first, alloc_position);
    return true;
}
}
