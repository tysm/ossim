#pragma once
#include <vector>
#include <memory>
#include <utility>

namespace sosim
{
template <class T, class U>
using vector_pair = std::vector<std::pair<T, U> >;

/// The process structure.
struct Process
{
    unsigned bornTime;
    unsigned execTime;
    unsigned deadline;

    unsigned pid;

    /// The process page references:
    ///
    /// Each reference is a pair, where the first is the address and the second
    /// and the second represents the index of the process page.
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
