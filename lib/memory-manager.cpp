#include <sosim/memory-manager.hpp>

namespace sosim
{
auto MemoryManager::run() -> std::unique_ptr<Process>
{
    if(process)
    {
        // Trying some free alocation
        while(victim_page == size_t(-1) &&
              (*alloc_buffer.front()).first == size_t(-1) &&
              alloc(process->pid, *alloc_buffer.front()))
        {
            delay -= shift_delay;
            alloc_buffer.pop_front();
        }

        if(delay)
        {
            // We need to find some victim page
            if(victim_page == size_t(-1))
                choose_victim();

            // There's a victim page, so we can perform pagination
            if(victim_page != size_t(-1))
            {
                delay--;
                if(delay%shift_delay == 0)
                    pagination();
            }
        }

        if(!delay)
            return std::move(process);
    }
    return nullptr;
}

void MemoryManager::choose_victim()
{
    // Looking for some valid victim in 'ram'
    victim_page = this->alloc_position(process->pid);
    if(!valid_alloc_position(victim_page, process->pid))
    {
        regress_alloc_position();
        victim_page = -1;
    }
    else
    {
        auto &ref = *alloc_buffer.front();
        // Checking if the 'ref' needs 'page_table' space
        if(ref.first == size_t(-1))
        {
            auto virtual_position = this->virtual_position();
            if(!valid_virtual_position(virtual_position))
            {
                regress_alloc_position();
                regress_virtual_position();
                victim_page = -1;
            }
            else
                ref.first = virtual_position;
        }

        if(ref.first != size_t(-1))
        {
            victim_ref = was_allocated(victim_page);
            // Invalidating the last 'victim_page' reference in 'page_table'
            if(victim_ref != size_t(-1))
                page_table[victim_ref].second = false;

            // Preparing the 'page_table'
            page_table[ref.first] = {victim_page, false};

            // New access successfully
            make_updates(ref.first, victim_page);
        }
    }
}

void MemoryManager::pagination()
{
    auto &ref = *alloc_buffer.front();

    // Checking if the referenced page is in 'swap'
    auto it = swap.begin();
    while(it != swap.end() && ((*it).first != process->pid || (*it).second != ref.second))
        it++;
    // Then remove it
    if(it != swap.end())
        swap.erase(it);

    // Sending the 'victim_page' to 'swap'
    if(victim_ref != size_t(-1))
        swap.push_back(ram[victim_page]);

    // Inserting the referenced page in 'ram'
    ram[victim_page] = {process->pid, ref.second};
    // Updating the 'page_table'
    page_table[ref.first].second = true;

    alloc_buffer.pop_front();
    victim_page = -1;
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
        if(ref.first == size_t(-1) && !alloc(pid, ref))
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

bool MemoryManager::alloc(unsigned pid, std::pair<size_t, size_t> &ref)
{
    auto virtual_position = this->virtual_position();
    // Fully allocated virtual memory
    if(!valid_virtual_position(virtual_position))
    {
        regress_virtual_position();
        return false;
    }

    auto alloc_position = this->alloc_position(pid);
    // Fully allocated RAM
    if(!valid_alloc_position(alloc_position, pid))
    {
        regress_virtual_position();
        regress_alloc_position();
        // Should the system crash?
        return false;
    }

    auto used_ref = was_allocated(alloc_position);
    // Position is ocupied
    if(used_ref != size_t(-1))
    {
        regress_virtual_position();
        regress_alloc_position();
        return false;
    }

    ref.first = virtual_position;

    // Allocating in ram
    ram[alloc_position] = {pid, ref.second};
    page_table[ref.first] = {alloc_position, true};

    // New access successfully
    make_updates(ref.first, alloc_position);
    return true;
}
}
