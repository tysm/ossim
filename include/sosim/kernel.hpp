#pragma once
#include <list>
#include <vector>
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
                    std::list<std::shared_ptr<Process> > blocked) :
        cpu(std::move(cpu)), scheduler(std::move(scheduler)),
        mManager(std::move(mManager)), push_requests(),
        blocked(std::move(blocked)),
        self(std::make_shared<Process>(0, -1, -1, -1, 0, 0, 0))
    {
    }

    void run();

    void push(std::shared_ptr<Process> process)
    {
        push_requests.push_back(std::move(process));
    }

    auto get_ready() -> std::vector<std::shared_ptr<Process> >
    {
        return scheduler->get_ready();
    }

    auto get_blocked() -> std::vector<std::shared_ptr<Process> >
    {
        return std::vector<std::shared_ptr<Process> >(blocked.begin(),
                                                      blocked.end());
    }

private:
    void next();

    CPU cpu;

    std::unique_ptr<Scheduler> scheduler;
    std::unique_ptr<MemoryManager> mManager;

    std::list<std::shared_ptr<Process> > push_requests;
    std::list<std::shared_ptr<Process> > blocked;

    unsigned quantum;
    unsigned overload;

    std::shared_ptr<Process> self;
};
}
