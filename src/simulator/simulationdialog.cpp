#include "simulationdialog.h"
#include "ui_simulationdialog.h"
using namespace sosim;

static const int colorTable[] = {
    Qt::red,
    Qt::green,
    Qt::blue,
    Qt::cyan,
    Qt::magenta,
    Qt::yellow,
    Qt::darkRed,
    Qt::darkGreen,
    Qt::darkBlue,
    Qt::darkCyan,
    Qt::darkMagenta,
    Qt::darkYellow,
    Qt::lightGray,
    Qt::gray,
    Qt::darkGray,
    Qt::black,
};

SimulationDialog::SimulationDialog(std::unique_ptr<sosim::Simulator> sim,
                                   QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationDialog),
    sim(std::move(sim))
{
    ui->setupUi(this);
}


SimulationDialog::~SimulationDialog()
{
    delete ui;
}

int SimulationDialog::exec()
{
    this->timer = this->startTimer(1000);
    this->performTick();
    return QDialog::exec();
}

void SimulationDialog::timerEvent(QTimerEvent* event)
{
    if(sim->cpu_state() == CPUState::Idle && sim->remaining_processes() == 0)
    {
        this->killTimer(this->timer);
        fprintf(stderr, "SIMULATION IS OVER PID %d TIME %d STATE %d REMAINING %d\n",
                this->sim->cpu_pid(), this->sim->cpu_time(),
                (int) this->sim->cpu_state(),
                sim->remaining_processes());
        return;
    }

    this->performTick();
}

void SimulationDialog::performTick()
{
    fputs("RUN\n", stderr);
    this->sim->run();
    fputs("updateScreen\n", stderr);
    this->updateScreen();
    fputs("TIME\n", stderr);
    this->sim->time();
    fputs("END OF STEP\n", stderr);
}

void SimulationDialog::updateScreen()
{
    this->updateTimeline();
    this->updateRAM();
    this->updateDisk();
    this->updatePageTable();
}

void SimulationDialog::updateTimeline()
{
    fprintf(stderr, "DOING SIMULATION PID %d TIME %d STATE %d\n",
            this->sim->cpu_pid(), this->sim->cpu_time(),
            (int) this->sim->cpu_state());

    auto column = this->sim->cpu_time() + 1;
    auto row = this->sim->cpu_pid();
    auto color = Qt::yellow;

    auto procTable = this->findChild<QTableWidget*>("procTable");

    switch(this->sim->cpu_state())
    {
        case CPUState::Exec:
            color = this->sim->is_over_deadline()? Qt::gray : Qt::green;
            break;
        case CPUState::Overload:
            color = Qt::red;
            break;
        case CPUState::Idle:
            color = Qt::yellow;
            break;
    }

    if(color != Qt::yellow)
    {
        auto item = new QTableWidgetItem();
        item->setBackgroundColor(color);
        procTable->setItem(row - 1, column - 1, item);
    }
    else
    {
        for(int i = 0; i < procTable->columnCount(); ++i)
        {
            auto item = new QTableWidgetItem();
            item->setBackgroundColor(color);
            procTable->setItem(i, column - 1, item);
        }
    }
}

void SimulationDialog::updateRAM()
{
    const size_t colorTableSize = sizeof(colorTable) / sizeof(*colorTable);

    auto ramTable = this->findChild<QTableWidget*>("ramTable");

    auto& memgr = sim->get_memory_manager();
    for(int i = 0; i < memgr.ram.size(); ++i)
    {
        auto row = i % 10;
        auto column = i / 10;
        auto pid = memgr.ram[i].first;

        auto color = colorTable[pid % colorTableSize];

        auto item = new QTableWidgetItem();
        item->setBackgroundColor(color);
        ramTable->getItem(row, column)->setBackgroundColor(color);
        ramTable->setItem(row, column, item);
    }
}

void SimulationDialog::updateDisk()
{

}

void SimulationDialog::updatePageTable()
{

}
