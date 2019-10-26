#include "simulatorgui.h"
#include "processdialog.h"
#include "simulationdialog.h"
#include "ui_simulatorgui.h"
#include <ossim/simulator.hpp>
using namespace ossim;

SimulatorGUI::SimulatorGUI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SimulatorGUI) {
  ui->setupUi(this);
}

SimulatorGUI::~SimulatorGUI() { delete ui; }

void SimulatorGUI::on_btn_next_released() {
  auto simuDelay = this->findChild<QSpinBox *>("delay")->value();
  auto numProcs = this->findChild<QSpinBox *>("nProcesses")->value();
  auto quantum = this->findChild<QSpinBox *>("quantum")->value();
  auto overload = this->findChild<QSpinBox *>("overload")->value();
  auto shiftDelay = this->findChild<QSpinBox *>("pagingDelay")->value();
  auto virtualPages = this->findChild<QSpinBox *>("nVirtualPages")->value();
  auto ramPages = this->findChild<QSpinBox *>("nRAMPages")->value();

  auto scheduler = SchedulerKind::FIFO_S;
  if (this->findChild<QPushButton *>("FIFO_S")->isChecked())
    scheduler = SchedulerKind::FIFO_S;
  else if (this->findChild<QPushButton *>("SJF")->isChecked())
    scheduler = SchedulerKind::SJF;
  else if (this->findChild<QPushButton *>("EDF")->isChecked())
    scheduler = SchedulerKind::EDF;
  else if (this->findChild<QPushButton *>("RoundRobin")->isChecked())
    scheduler = SchedulerKind::RoundRobin;

  auto mman = MemoryManagerKind::FIFO_MM;
  if (this->findChild<QPushButton *>("FIFO_MM")->isChecked())
    mman = MemoryManagerKind::FIFO_MM;
  else if (this->findChild<QPushButton *>("LRU")->isChecked())
    mman = MemoryManagerKind::LRU;

  auto sim = std::make_unique<ossim::Simulator>();
  sim->set_kernel(scheduler, quantum, overload, mman, shiftDelay, virtualPages,
                  ramPages);

  for (int i = 0; i < numProcs; ++i) {
    ProcessDialog processDialog(this);
    processDialog.findChild<QSpinBox *>("pid")->setValue(i + 1);

    if (processDialog.exec() == QDialog::Rejected)
      return;

    auto pid = processDialog.findChild<QSpinBox *>("pid")->value();
    auto bornTime = processDialog.findChild<QSpinBox *>("bornTime")->value();
    auto execTime = processDialog.findChild<QSpinBox *>("executionTime")->value();
    auto deadline = processDialog.findChild<QSpinBox *>("deadline")->value();
    auto nPages = processDialog.findChild<QSpinBox *>("nPages")->value();
    sim->push(bornTime, execTime, deadline, pid, nPages);
  }

  SimulationDialog simulatorDialog(std::move(sim), simuDelay, this);
  simulatorDialog.exec();
}
