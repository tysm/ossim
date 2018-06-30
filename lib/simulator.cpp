#include <sosim/simulator.hpp>

#ifdef __unix__
#include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define sleep(n) Sleep(n*int(1e3))
#endif

namespace sosim
{
void Simulator::run()
{
    cpu->run();
    while(!buffer.empty() && buffer.front()->bornTime <= current_time)
    {
        kernel->push(std::move(buffer.front()));
        buffer.pop_front();
    }
    kernel->run();
}

void Simulator::time()
{
    sleep(delay);
    current_time += delay;
}
}
