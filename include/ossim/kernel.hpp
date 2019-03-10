#pragma once
#include <list>
#include <memory>
#include <ossim/commom.hpp>
#include <ossim/cpu.hpp>
#include <ossim/memory-manager.hpp>
#include <ossim/scheduler.hpp>
#include <utility>

namespace ossim {
class Kernel {
public:
  explicit Kernel(std::shared_ptr<CPU> cpu,
                  std::unique_ptr<Scheduler> scheduler, unsigned quantum,
                  unsigned overload, std::unique_ptr<MemoryManager> memMng)
      : cpu(std::move(cpu)), scheduler(std::move(scheduler)), quantum(quantum),
        overload(overload), memMng(std::move(memMng)),
        self(std::make_unique<Process>(0, -1, -1, 0, 0)) {}

  void run();

  void push(std::unique_ptr<Process> process) {
    push_requests.push_back(std::move(process));
  }

  auto remaining_processes() -> size_t {
    size_t remaining = 0;
    remaining += scheduler->remaining_processes();
    remaining += cpu->state() == CPUState::Exec ? 1 : 0;
    remaining += blocked.size();
    remaining += memMng->state() == MemoryManagerState::Busy ? 1 : 0;
    return remaining;
  }

  auto get_scheduler() -> const Scheduler & { return *this->scheduler; }

  auto get_memory_manager() -> const MemoryManager & { return *memMng; }

private:
  void next();

  std::shared_ptr<CPU> cpu;

  std::unique_ptr<Scheduler> scheduler;

  unsigned quantum;
  unsigned overload;
  unsigned quantum_counter;
  unsigned overload_counter;

  std::unique_ptr<MemoryManager> memMng;

  std::list<std::unique_ptr<Process>> push_requests;
  std::list<std::unique_ptr<Process>> blocked;

  std::unique_ptr<Process> self;
};
} // namespace ossim
