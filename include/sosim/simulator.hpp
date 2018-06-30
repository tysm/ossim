#pragma once
#include <list>
#include <memory>
#include <utility>
#include <sosim/commom.hpp>
#include <sosim/cpu.hpp>
#include <sosim/kernel.hpp>

namespace sosim
{
class Simulator
{
public:
    explicit Simulator() :
        cpu(std::make_shared<CPU>(CPU())), current_time(0)
    {
    }

    // TODO input

    void run();

    void time();

private:
    std::shared_ptr<CPU> cpu;
    std::unique_ptr<Kernel> kernel;

    unsigned current_time;
    unsigned delay;

    std::list<std::unique_ptr<Process> > buffer;
};
}
