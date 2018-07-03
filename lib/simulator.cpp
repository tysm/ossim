#include <sosim/simulator.hpp>

#ifdef __unix__
#include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep(n) Sleep(n*int(1e3))
#endif

namespace sosim
{
void Simulator::run()
{
    while(!buffer.empty() && buffer.front()->bornTime <= current_time)
    {
        kernel->push(std::move(buffer.front()));
        buffer.pop_front();
    }
    kernel->run();
}

void Simulator::time()
{
    sleep(delay);
    current_time++;
    if(auto process = cpu->run())
        runtime += current_time - process->bornTime;
}
void Simulator::set_kernel(SchedulerKind sKind, unsigned quantum,
                           unsigned overload, MemoryManagerKind mmKind,
                           unsigned shift_delay, size_t virtual_pages,
                           size_t ram_pages)
{
    cpu = std::make_shared<CPU>();

    std::unique_ptr<Scheduler> scheduler;
    switch(sKind)
    {
        case SchedulerKind::FIFO_S:
            scheduler = std::make_unique<FIFO_S>();
            break;
        case SchedulerKind::SJF:
            scheduler = std::make_unique<SJF>();
            break;
        case SchedulerKind::RoundRobin:
            scheduler = std::make_unique<RoundRobin>();
            break;
        case SchedulerKind::EDF:
            scheduler = std::make_unique<EDF>();
            break;
        default:
            break;
    }

    std::unique_ptr<MemoryManager> memMng;
    switch(mmKind)
    {
        case MemoryManagerKind::FIFO_MM:
            memMng = std::make_unique<FIFO_MM>(shift_delay, virtual_pages,
                                               ram_pages);
            break;
        case MemoryManagerKind::LRU:
            memMng = std::make_unique<LRU>(shift_delay, virtual_pages,
                                           ram_pages);
            break;
        default:
            break;
    }
    kernel = std::make_unique<Kernel>(cpu, std::move(scheduler), quantum,
                                      overload, std::move(memMng));
}
}
