#pragma once
#include <queue>
#include <vector>
#include <memory>
#include <utility>
#include <algorithm>
#include <sosim/commom.hpp>

namespace sosim
{
class Comparator
{
public:
    virtual bool operator() (const std::unique_ptr<Process> &i,
                             const std::unique_ptr<Process> &j)
    {
        return false;
    }
};

class ComparatorSJF : public Comparator
{
public:
    bool operator() (const std::unique_ptr<Process> &i,
                     const std::unique_ptr<Process> &j) override
    {
        return i->execTime < j->execTime;
    }
};

class ComparatorEDF : public Comparator
{
public:
    bool operator() (const std::unique_ptr<Process> &i,
                     const std::unique_ptr<Process> &j) override
    {
        return i->deadline < j->deadline;
    }
};

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
        auto process = std::move(this->ready.front());
        this->ready.pop();
        return process;
    }

    virtual bool is_preemptive()
    {
        return false;
    }

private:
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
    std::priority_queue<std::unique_ptr<Process>,
        std::vector<std::unique_ptr<Process> >,
        ComparatorSJF> ready;
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
    std::priority_queue<std::unique_ptr<Process>,
        std::vector<std::unique_ptr<Process> >,
        ComparatorEDF> ready;
};
}
