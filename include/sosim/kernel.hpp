#pragma once
#include <list>
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
    explicit Kernel(CPU cpu, Scheduler scheduler, MemoryManager mManager) :
        cpu(std::move(cpu)), scheduler(std::move(scheduler)),
        mManager(std::move(mManager)), push_requests(),
        self(make_shared<Process>(0, -1, -1, -1, 0, 0))
    {
    }

    void run();

    void push(std::shared_ptr<Process> process)
    {
        push_requests.push_back(std::move(process));
    }

private:
    void next();

    CPU cpu;

    Scheduler scheduler;
    MemoryManager mManager;

    std::list<std::shared_ptr<Process> > push_requests;

    unsigned quantum;
    unsigned overload;

    std::shared_ptr<Process> self;
};
}
