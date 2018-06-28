#pragma once
#include <vector>

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

    std::vector<int> page_refs;


    explicit Process(unsigned bornTime, unsigned execTime, unsigned deadline,
                     unsigned quantum, unsigned overload, unsigned pid,
                     int nPages) :
        bornTime(bornTime), execTime(execTime), deadline(deadline),
        quantum(quantum), pid(pid), page_refs(nPages, -1)
    {
    }
};
}
