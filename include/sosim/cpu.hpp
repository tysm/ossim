#pragma once
#include <memory>
#include <utility>
#include <sosim/commom.hpp>

namespace sosim
{
enum class CPUState
{
    Exec,
    Overload,
    Idle,
};

class CPU
{
public:
    explicit CPU() :
        process()
    {
    }

    void run()
    {
        if(process)
        {
            process->execTime--;
            if(process->execTime == 0)
                process.reset();
        }
    }

    void push(std::unique_ptr<Process> process)
    {
        this->process = std::move(process);
    }

    auto drop() -> std::unique_ptr<Process>
    {
        return std::move(this->process);
    }

    CPUState state()
    {
        if(!process)
            return CPUState::Idle;
        // If the pid is 0 (i.e. the kernel), then we are in overload state.
        return !process->pid? CPUState::Overload : CPUState::Exec;
    }

private:
    std::unique_ptr<Process> process;
};
}
