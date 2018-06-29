#pragma once
#include <list>
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

    void push(std::shared_ptr<Process> process)
    {
        ready.push_back(std::move(process));
        std::stable_sort(ready.begin(), ready.end(), comparator);
    }

    auto next() -> std::shared_ptr<Process>
    {
        if(ready.empty())
            return nullptr;
        auto process = std::move(ready.front());
        ready.pop_front();
        return process;
    }

    auto get_ready() -> std::vector<std::shared_ptr<Process> >
    {
        return std::vector<std::shared_ptr<Process> >(ready.begin(),
                                                      ready.end());
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
                    const std::shared_ptr<Process> &j) override
    {
        return i->execTime < j->execTime;
    }
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
    bool comparator(const std::shared_ptr<Process> &i,
                    const std::shared_ptr<Process> &j) override
    {
        return i->deadline < j->deadline;
    }
};
}
