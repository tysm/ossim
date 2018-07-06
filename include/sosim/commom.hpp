#pragma once
#include <vector>
#include <memory>

namespace sosim
{
template <class T, class U>
using vector_pair = std::vector<std::pair<T, U> >;

struct Process
{
    unsigned bornTime;
    unsigned execTime;
    unsigned deadline;

    unsigned pid;

    std::shared_ptr<vector_pair<size_t, size_t> > page_refs;


    explicit Process(unsigned bornTime, unsigned execTime, unsigned deadline,
                     unsigned pid, size_t nPages) :
        bornTime(bornTime), execTime(execTime), deadline(deadline), pid(pid),
        page_refs(std::make_shared<vector_pair<size_t, size_t> >(nPages))
    {
        for(size_t i = 0; i < nPages; ++i)
        {
            (*page_refs)[i] = {-1, i};
        }
    }
};
}
