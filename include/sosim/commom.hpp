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

    std::vector<size_t> page_refs;


    explicit Process(unsigned bornTime, unsigned execTime, unsigned deadline,
                     unsigned quantum, unsigned overload, unsigned pid,
                     size_t nPages) :
        bornTime(bornTime), execTime(execTime), deadline(deadline),
        quantum(quantum), overload(overload), pid(pid), page_refs(nPages, -1)
    {
    }
};
}
