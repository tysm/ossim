#include <memory>
#include <sosim/simulator.hpp>
using namespace sosim;

int main()
{
// TODO init display
// TODO while display
// {
    auto simulator = std::make_unique<Simulator>();
// TODO input display -> simulator
    while(simulator->cpu_state() != CPUState::Idle &&
          simulator->remaining_processes() != 0)
    {
        simulator->run();
// TODO simulator -> output display
        simulator->time();
    }
// TODO wait for end display
// }
    return 0;
}
