#include <ossim/kernel.hpp>

namespace ossim {
void Kernel::run() {
  // Dealloc the last process
  if (cpu->state() == CPUState::Idle)
    memMng->dealloc(false);

  // Looking for unblocked processes
  while (!blocked.empty() && memMng->state() == MemoryManagerState::Free) {
    auto process = std::move(blocked.front());
    if (!memMng->try_alloc(process->pid, process->page_refs, true))
      memMng->push(std::move(process));
    else
      scheduler->push(std::move(process));
    blocked.pop_front();
  }

  // Inserting new processes
  while (!push_requests.empty()) {
    scheduler->push(std::move(push_requests.front()));
    push_requests.pop_front();
  }

  // Trying to perform exchanges at CPU
  if (cpu->state() == CPUState::Idle)
    this->next();
  else if (scheduler->is_preemptive()) {
    if (quantum_counter)
      quantum_counter--;
    else if (overload_counter) {
      overload_counter--;
      if (cpu->state() != CPUState::Overload) {
        scheduler->push(cpu->drop());
        cpu->push(std::move(self));
      }
    } else {
      memMng->dealloc(true);
      if (auto process = cpu->drop())
        self = std::move(process);
      this->next();
    }
  }

  // Running the Memory Manager and maybe unblocking some process
  if (auto process = memMng->run())
    scheduler->push(std::move(process));
}

void Kernel::next() {
  while (cpu->state() == CPUState::Idle) {
    auto process = scheduler->next();

    if (!process)
      return;

    // Checking if the process is allocated
    if (memMng->try_alloc(process->pid, process->page_refs, false)) {
      if (scheduler->is_preemptive()) {
        quantum_counter = quantum - 1;
        overload_counter = overload;
      }
      cpu->push(std::move(process));
    } else {
      if (memMng->state() == MemoryManagerState::Free)
        memMng->push(std::move(process));
      else
        blocked.push_back(std::move(process));
    }
  }
}
} // namespace ossim
