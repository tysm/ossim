#include <sosim/kernel.hpp>

namespace sosim
{
void Kernel::run()
{
    // Running the Memory Manager and perhaps unblocking some process
    if(auto process = mManager.run())
        scheduler.push(std::move(process));
    
    // Inserting new processes
    for(auto process : push_requests)
        scheduler.push(std::move(process));
    push_requests.clear();
    
    // Sorting ready processes by the scheduler
    scheduler.run();
    
    if(cpu.state() == Idle)
        this->next();
    else if(scheduler.is_preemptive())
    {
        if(quantum)
            quantum--;
        else if(overload)
        {
            overload--;
            scheduler.push(cpu.drop());
            cpu.push(std::move(self));
        }
        else
        {
            if(auto process = cpu.drop())
                self = std::move(process);
            this->next();
        }
    }
}

void next()
{
    // Looking for some really ready process
    auto process = scheduler.next();
    while(process && !(process = mManager.check(std::move(process))))
        process = scheduler.next();

    // If there is a ready process then push it on cpu
    if(process)
    {
        if(scheduler.is_preemptive())
        {
            quantum = process->quantum;
            overload = process->overload;
        }
        cpu.push(std::move(process));
    }
}
}
