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
    explicit Kernel(CPU cpu, Scheduler scheduler, MemoryManager mManager,
                    std::queue<std::shared_ptr<Process> > &blocked) :
        cpu(std::move(cpu)), scheduler(std::move(scheduler)),
        mManager(std::move(mManager)), push_requests(),
        self(std::make_shared<Process>(0, -1, -1, -1, 0, 0),
        blocked(std::move(blocked))
    {
    }

    void run();

    void push(std::shared_ptr<Process> process)
    {
        push_requests.push(std::move(process));
    }

private:
    void next();

    CPU cpu;

    Scheduler scheduler;
    MemoryManager mManager;

    std::queue<std::shared_ptr<Process> > push_requests;
    std::queue<std::shared_ptr<Process> > blocked;

    unsigned quantum;
    unsigned overload;

    std::shared_ptr<Process> self;
};
}
