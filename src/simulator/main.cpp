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
    do
    {
        simulator->run();
// TODO simulator -> output display
        simulator->time();
    } while(simulator->cpu_state() != CPUState::Idle ||
            simulator->remaining_processes() != 0);
// TODO wait for end display
// }
    return 0;
}
