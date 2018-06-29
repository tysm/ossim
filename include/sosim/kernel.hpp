#pragma once
#include <queue>
#include <memory>
#include <utility>
#include <sosim/commom.hpp>
#include <sosim/cpu.hpp>
#include <sosim/scheduler.hpp>
#include <sosim/memory-manager.hpp>

namespace sosim
{
class Kernel
{
public:
    explicit Kernel(CPU cpu, std::unique_ptr<Scheduler> scheduler,
                    std::unique_ptr<MemoryManager> mManager,
                    std::queue<std::unique_ptr<Process> > &blocked) :
        cpu(std::move(cpu)), scheduler(std::move(scheduler)),
        mManager(std::move(mManager)), push_requests(),
        self(std::make_unique<Process>(0, -1, -1, -1, 0, 0),
        blocked(std::move(blocked))
    {
    }

    void run();

    void push(std::unique_ptr<Process> process)
    {
        push_requests.push(std::move(process));
    }

private:
    void next();

    CPU cpu;

    std::unique_ptr<Scheduler> scheduler;
    std::unique_ptr<MemoryManager> mManager;

    std::queue<std::unique_ptr<Process> > push_requests;
    std::queue<std::unique_ptr<Process> > blocked;

    unsigned quantum;
    unsigned overload;

    std::unique_ptr<Process> self;
};
}
