#pragma once
#include <vector>
#include <memory>

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

    std::shared_ptr<std::vector<size_t> > page_refs;


    explicit Process(unsigned bornTime, unsigned execTime, unsigned deadline,
                     unsigned quantum, unsigned overload, unsigned pid,
                     size_t nPages) :
        bornTime(bornTime), execTime(execTime), deadline(deadline),
        quantum(quantum), overload(overload), pid(pid),
        page_refs(std::make_shared<std::vector<size_t> >(nPages, -1))
    {
    }
};
}
