#include "simulatorgui.h"
#include "ui_simulatorgui.h"
#include "processdialog.h"
#include "simulationdialog.h"

SimulatorGUI::SimulatorGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SimulatorGUI)
{
    ui->setupUi(this);
}

SimulatorGUI::~SimulatorGUI()
{
    delete ui;
}

void SimulatorGUI::on_Next_clicked()
{
    auto nProcs = this->findChild<QSpinBox*>("nProcs")->value();
    auto quantum = this->findChild<QSpinBox*>("quantumBox")->value();
    auto overload = this->findChild<QSpinBox*>("overloadBox")->value();
    auto shift_delay = this->findChild<QSpinBox*>("pDelay")->value();
    auto virtual_pages = this->findChild<QSpinBox*>("nVirtual")->value();
    auto ram_pages = this->findChild<QSpinBox*>("nRAM")->value();

    SchedulerKind sKind = SchedulerKind::FIFO_S;
    if(this->findChild<QPushButton*>("FIFO_S")->isChecked())
        sKind = SchedulerKind::FIFO_S;
    else if(this->findChild<QPushButton*>("SJF")->isChecked())
        sKind = SchedulerKind::SJF;
    else if(this->findChild<QPushButton*>("EDF")->isChecked())
        sKind = SchedulerKind::EDF;
    else if(this->findChild<QPushButton*>("RoundRobin")->isChecked())
        sKind = SchedulerKind::RoundRobin;

    MemoryManagerKind mmKind = MemoryManagerKind::FIFO_MM;
    if(this->findChild<QPushButton*>("FIFO_MM")->isChecked())
        mmKind = MemoryManagerKind::FIFO_MM;
    else if(this->findChild<QPushButton*>("LRU")->isChecked())
        mmKind = MemoryManagerKind::LRU;

    this->sosim = std::make_unique<Simulator>();
    this->sosim->set_processes(nProcs);
    this->sosim->set_kernel(sKind, quantum, overload, mmKind, shift_delay, virtual_pages, ram_pages);

    for(int i = 0; i < nProcs; ++i)
    {
        auto processDialog = std::make_unique<ProcessDialog>(this);
        processDialog->findChild<QSpinBox*>("pidBox")->setValue(i + 1);
        if(processDialog->exec() == QDialog::Rejected)
            return;

        auto pid = processDialog->findChild<QSpinBox*>("pidBox")->value();
        auto bornTime = processDialog->findChild<QSpinBox*>("bornBox")->value();
        auto execTime = processDialog->findChild<QSpinBox*>("execBox")->value();
        auto deadline = processDialog->findChild<QSpinBox*>("deadBox")->value();
        auto nPages = processDialog->findChild<QSpinBox*>("nPagesBox")->value();
        this->sosim->push(bornTime, execTime, deadline, pid, nPages);
    }

    auto simdiag = std::make_unique<SimulationDialog>(std::move(sosim), this);
    simdiag->exec();
}



