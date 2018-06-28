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

    auto run() -> std::shared_ptr<Process>
    {
        if(process)
        {
            process->execTime--;
            if(process->execTime == 0)
                return process = nullptr;
        }
        return process;
    }

    void push(std::shared_ptr<Process> process) :
    {
        this->process = std::move(process);
    }

    auto drop() -> shared_ptr<Process>
    {
        return std::move(this->process);
    }

    CPUState state()
    {
        if(!process)
            return Idle;
        return !process->pid? Exec : Overload;
    }

private:
    std::shared_ptr<Process> process;
};
}
