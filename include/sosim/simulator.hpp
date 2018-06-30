#pragma once
#include <list>
#include <memory>
#include <utility>
#include <sosim/commom.hpp>
#include <sosim/cpu.hpp>
#include <sosim/kernel.hpp>

namespace sosim
{
class Simulator
{
public:
    // TODO input

    void run();

    void time();

    auto cpu_state() -> CPUState
    {
        return cpu->state();
    }

    auto remaining_processes() -> size_t
    {
        return kernel->remaining_processes();
    }

    auto runtime_per_process() -> long double
    {
        if(auto conclued = processes - remaining_processes())
            return runtime/conclued;
        return 0;
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
