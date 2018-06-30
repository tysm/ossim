#include <memory>
#include <sosim/simulator.hpp>
using namespace sosim;

int main()
{
    auto simulator = std::make_unique<Simulator>();

    // TODO input and display

    while(simulator->cpu_state() != CPUState::Idle &&
          simulator->remaining_processes() != 0)
    {
        simulator->run();
        // TODO display
        simulator->time();
    }
    return 0;
}
