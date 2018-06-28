#pragma once
#include <list>

namespace sosim
{
struct Process
{
    unsigned bornTime;
    unsigned execTime;
    unsigned deadline;

    unsigned quantum;
    unsigned overload;

    unsigned pid;

    std::list<int> page_refs;


    explicit Process(const unsigned bornTime, unsigned execTime,
                     const unsigned deadline, const unsigned quantum,
                     const unsigned overload, const unsigned pid,
                     const int nPages) :
        bornTime(bornTime), execTime(execTime), deadline(deadline),
        quantum(quantum), pid(pid), page_refs(nPages, -1)
    {
    }
};
}
