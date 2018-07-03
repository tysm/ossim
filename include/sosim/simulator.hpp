#pragma once
#include <list>
#include <memory>
#include <utility>
#include <sosim/commom.hpp>
#include <sosim/cpu.hpp>
#include <sosim/scheduler.hpp>
#include <sosim/memory-manager.hpp>
#include <sosim/kernel.hpp>

namespace sosim
{
class Simulator
{
public:
    explicit Simulator() : current_time(0), runtime(0)
    {
    }

    void run();

    void time();

    auto runtime_per_process() -> long double
    {
        if(auto conclued = processes - remaining_processes())
            return runtime/conclued;
        return 0;
    }

    auto cpu_state() -> CPUState
    {
        return cpu->state();
    }

    auto remaining_processes() -> size_t
    {
        return kernel->remaining_processes();
    }

    void set_kernel(SchedulerKind sKind, unsigned quantum, unsigned overload,
                    MemoryManagerKind mmKind, unsigned shift_delay,
                    size_t virtual_pages, size_t ram_pages);

    void set_processes(size_t processes)
    {
        this->processes = processes;
    }

    void set_delay(unsigned delay)
    {
        this->delay = delay;
    }

    void push(unsigned bornTime, unsigned execTime, unsigned deadline,
              unsigned pid, size_t nPages)
    {
        auto process = std::make_unique<Process>(bornTime, execTime, deadline,
                                                 pid, nPages);
        buffer.push_back(std::move(process));
    }

private:
    std::shared_ptr<CPU> cpu;
    std::unique_ptr<Kernel> kernel;

    size_t processes;

    unsigned current_time;
    unsigned delay;

    unsigned long long runtime;

    std::list<std::unique_ptr<Process> > buffer;
};
}
