#pragma once
#include <queue>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <sosim/commom.hpp>

namespace sosim
{
class Scheduler
{
public:
    explicit Scheduler() :
        ready()
    {
    }

    void push(std::unique_ptr<Process> process)
    {
        this->ready.push(std::move(process));
    }

    auto next() -> std::unique_ptr<Process>
    {
        if(this->ready.empty())
            return nullptr;
        auto process = this->ready.front();
        this->ready.pop();
        return process;
    }

    virtual bool is_preemptive()
    {
        return false;
    }

private:
    virtual bool comparator(const std::unique_ptr<Process> &i,
                            const std::unique_ptr<Process> &j)
    {
        return false;
    }

    std::queue<std::unique_ptr<Process> > ready;
};

class NotPreemptive : public Scheduler
{
};

class FIFO : public NotPreemptive
{
};

class SJF : public NotPreemptive
{
private:
    bool comparator(const std::unique_ptr<Process> &i,
                    const std::unique_ptr<Process> &j) override
    {
        return i->execTime < j->execTime;
    }

    std::priority_queue<std::unique_ptr<Process>,
        std::vector<std::unique_ptr<Process> >,
        std::function<bool(std::unique_ptr<Process>,
            std::unique_ptr<Process>) > > ready(this->comparator);
};

class Preemptive : public Scheduler
{
public:
    bool is_preemptive() override
    {
        return true;
    }
};

class RoundRobin : public Preemptive
{
};

class EDF : public Preemptive
{
private:
    bool comparator(const std::unique_ptr<Process> &i,
                    const std::unique_ptr<Process> &j) override
    {
        return i->deadline < j->deadline;
    }

    std::priority_queue<std::unique_ptr<Process>,
        std::vector<std::unique_ptr<Process> >,
        std::function<bool(std::unique_ptr<Process>,
            std::unique_ptr<Process>) > > ready(this->comparator);
};
}
