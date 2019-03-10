#pragma once
#include <memory>
#include <utility>
#include <vector>

namespace ossim {
template <class T, class U> using vector_pair = std::vector<std::pair<T, U>>;

/// The process structure.
struct Process {
  /// Every process must have:
  ///
  /// Its creation time aka 'born_time;
  /// Its execution time aka 'exec_time';
  /// Its deadline;
  /// Its process ID aka 'pid';
  /// Its page references aka 'page_refs';

  unsigned born_time;
  unsigned exec_time;
  unsigned deadline;

  unsigned pid;

  /// The process page references:
  ///
  /// Each reference is a pair, where the first is the address
  /// and the second represents the index of the process page.
  std::shared_ptr<vector_pair<size_t, size_t>> page_refs;

  explicit Process(unsigned born_time, unsigned exec_time, unsigned deadline,
                   unsigned pid, size_t nPages)
      : born_time(born_time), exec_time(exec_time), deadline(deadline),
        pid(pid),
        page_refs(std::make_shared<vector_pair<size_t, size_t>>(nPages)) {
    for (size_t i = 0; i < nPages; ++i) {
      (*page_refs)[i] = {-1, i};
    }
  }
};
} // namespace ossim
