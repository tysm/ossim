#pragma once
#include <list>
#include <memory>
#include <ossim/commom.hpp>
#include <ossim/cpu.hpp>
#include <ossim/kernel.hpp>
#include <ossim/memory-manager.hpp>
#include <ossim/scheduler.hpp>
#include <utility>

namespace ossim {
class Simulator {
public:
  explicit Simulator() : current_time(0), runtime(0) {}

  void run();

  void time();

  auto runtime_per_process() -> double {
    if (auto conclued = processes - remaining_processes())
      return double(runtime) / conclued;
    return 0;
  }

  auto cpu_state() -> CPUState { return cpu->state(); }

  auto cpu_pid() -> unsigned {
    if (auto proc = cpu->current_process()) {
      if (proc->pid != 0)
        return proc->pid;
    }
    return this->last_pid;
  }

  auto cpu_time() -> unsigned { return this->current_time; }

  bool is_over_deadline() const {
    if (auto proc = cpu->current_process()) {
      return kernel->get_scheduler().is_over_deadline(current_time, *proc);
    }
    return false;
  }

  auto get_memory_manager() -> const MemoryManager & {
    return kernel->get_memory_manager();
  }

  auto remaining_processes() -> size_t {
    return kernel->remaining_processes() + this->buffer.size();
  }

  void set_kernel(SchedulerKind sKind, unsigned quantum, unsigned overload,
                  MemoryManagerKind mmKind, unsigned shift_delay,
                  size_t virtual_pages, size_t ram_pages);

  void set_delay(unsigned delay) { this->delay = delay; }

  void push(unsigned born_time, unsigned exec_time, unsigned deadline,
            unsigned pid, size_t nPages) {
    auto process =
        std::make_unique<Process>(born_time, exec_time, deadline, pid, nPages);
    buffer.push_back(std::move(process));
    ++processes;
  }

private:
  std::shared_ptr<CPU> cpu;
  std::unique_ptr<Kernel> kernel;

  size_t processes = 0;

  unsigned current_time;
  unsigned delay;
  unsigned last_pid = 0;

  unsigned long long runtime;

  std::list<std::unique_ptr<Process>> buffer;
};
} // namespace ossim
