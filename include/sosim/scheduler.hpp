#pragma once
#include <list>
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

    void run()
    {
        std::stable_sort(ready.begin(), ready.end(), this->comparator);
    }

    void push(std::shared_ptr<Process> process)
    {
        ready.push_back(std::move(process));
    }

    auto next() -> std::shared_ptr<Process>
    {
        if(ready.empty())
            return nullptr;
        auto process = ready.front();
        ready.pop_front();
        return process;
    }

    virtual bool is_preemptive()
    {
        return false;
    }

private:
    virtual bool comparator(const std::shared_ptr<Process> &i,
                            const std::shared_ptr<Process> &j)
    {
        return false;
    }

    std::list<std::shared_ptr<Process> > ready;
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
    bool comparator(const std::shared_ptr<Process> &i,
                    const std::shared_ptr<Process> &j)
    {
        return i->execTime < j->execTime;
    }
};

class Preemptive : public Scheduler
{
public:
    bool is_preemptive()
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
    bool comparator(const std::shared_ptr<Process> &i,
                    const std::shared_ptr<Process> &j)
    {
        return i->deadline < j->deadline;
    }
};
}
