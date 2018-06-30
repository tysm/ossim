#pragma once
#include <list>
#include <memory>
#include <utility>
#include <algorithm>
#include <sosim/commom.hpp>

namespace sosim
{
class Scheduler;
class FIFO_S;
class SJF;
class RoundRobin;
class EDF;

enum class SchedulerKind
{
    FIFO_S,
    SJF,
    RoundRobin,
    EDF,
};

class Scheduler
{
public:
    void push(std::unique_ptr<Process> process)
    {
        ready.push_back(std::move(process));
        ready.sort([this](const std::unique_ptr<Process> &i,
                          const std::unique_ptr<Process> &j)
                         { return this->comparator(i, j); });
    }

    auto next() -> std::unique_ptr<Process>
    {
        if(ready.empty())
            return nullptr;
        auto process = std::move(ready.front());
        ready.pop_front();
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

    std::list<std::unique_ptr<Process> > ready;
};		

class NotPreemptive : public Scheduler
{
};

class FIFO_S : public NotPreemptive
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
};
}
