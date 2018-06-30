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
    explicit Kernel(std::shared_ptr<CPU> cpu,
                    std::unique_ptr<Scheduler> scheduler,
                    std::unique_ptr<MemoryManager> mManager,
                    shared_list<std::unique_ptr<Process> > blocked) :
        cpu(std::move(cpu)), scheduler(std::move(scheduler)),
        mManager(std::move(mManager)), blocked(std::move(blocked)),
        self(std::make_unique<Process>(0, -1, -1, -1, 0, 0, 0))
    {
    }

    void run();

    void push(std::unique_ptr<Process> process)
    {
        push_requests.push_back(std::move(process));
    }

    auto remaining_processes() -> size_t
    {
        return scheduler->remaining_processes() + blocked->size();
    }

private:
    void next();

    std::shared_ptr<CPU> cpu;

    std::unique_ptr<Scheduler> scheduler;
    std::unique_ptr<MemoryManager> mManager;

    std::list<std::unique_ptr<Process> > push_requests;
    shared_list<std::unique_ptr<Process> > blocked;

    unsigned quantum;
    unsigned overload;

    std::unique_ptr<Process> self;
};
}
