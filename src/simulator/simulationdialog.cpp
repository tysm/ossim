#include "simulationdialog.h"
#include "ui_simulationdialog.h"
using namespace sosim;

static const QColor colorTable[] = {
    // Qt::red,
    // Qt::green,
    Qt::blue,      Qt::cyan,     Qt::magenta,  Qt::yellow,      Qt::darkRed,
    Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
    Qt::lightGray, Qt::darkGray, Qt::black,
};

SimulationDialog::SimulationDialog(std::unique_ptr<sosim::Simulator> sim_,
                                   unsigned simulationTimeStep_,
                                   QWidget *parent)
    : QDialog(parent), ui(new Ui::SimulationDialog), sim(std::move(sim_)),
      simulationTimeStep(simulationTimeStep_) {
  ui->setupUi(this);

  this->timelineTable = this->findChild<QTableWidget *>("timelineTable");
  this->responseLabel = this->findChild<QLabel *>("responseLabel");
  this->ramTable = this->findChild<QTableWidget *>("ramTable");
  this->diskTable = this->findChild<QTableWidget *>("diskTable");
  this->pageTable = this->findChild<QTableWidget *>("pageTable");

  for (int i = 0; i < ramTable->rowCount(); ++i) {
    for (int j = 0; j < ramTable->columnCount(); ++j)
      ramTable->setItem(i, j, new QTableWidgetItem());
  }

  for (int i = 0; i < diskTable->rowCount(); ++i) {
    for (int j = 0; j < diskTable->columnCount(); ++j)
      diskTable->setItem(i, j, new QTableWidgetItem());
  }

  for (int i = 0; i < pageTable->rowCount(); ++i) {
    for (int j = 0; j < pageTable->columnCount(); ++j)
      pageTable->setItem(i, j, new QTableWidgetItem());
  }
}

SimulationDialog::~SimulationDialog() { delete ui; }

int SimulationDialog::exec() {
  responseLabel->setText("Executing...");
  this->timerHandle = this->startTimer(500 * this->simulationTimeStep);
  this->performTick();
  return QDialog::exec();
}

void SimulationDialog::timerEvent(QTimerEvent *event) {
  if (sim->cpu_state() == CPUState::Idle && sim->remaining_processes() == 0) {
    this->performTick();
    this->killTimer(this->timerHandle);
    responseLabel->setText(
        QString::asprintf("Turnaround: %f", sim->runtime_per_process()));
  } else {
    this->performTick();
  }
}

void SimulationDialog::performTick() {
  this->sim->run();
  this->updateScreen();
  this->sim->time();
}

void SimulationDialog::updateScreen() {
  this->updateTimeline();
  this->updateRAM();
  this->updateDisk();
  this->updatePageTable();
}

void SimulationDialog::updateTimeline() {
  auto column = this->sim->cpu_time() + 1;
  auto row = this->sim->cpu_pid();
  auto color = Qt::yellow;

  if (row >= timelineTable->rowCount() ||
      column >= timelineTable->columnCount())
    return;

  switch (this->sim->cpu_state()) {
  case CPUState::Exec:
    color = this->sim->is_over_deadline() ? Qt::gray : Qt::green;
    break;
  case CPUState::Overload:
    color = Qt::red;
    break;
  case CPUState::Idle:
    color = Qt::yellow;
    break;
  }

  if (color != Qt::yellow) {
    auto item = new QTableWidgetItem();
    item->setBackgroundColor(color);
    timelineTable->setItem(row - 1, column - 1, item);
  } else {
    if (sim->remaining_processes() != 0) {
      for (int i = 0; i < timelineTable->rowCount(); ++i) {
        auto item = new QTableWidgetItem();
        item->setBackgroundColor(color);
        timelineTable->setItem(i, column - 1, item);
      }
    }
  }
}

void SimulationDialog::updateRAM() {
  const auto colorTableSize = sizeof(colorTable) / sizeof(*colorTable);

  auto &memgr = sim->get_memory_manager();
  for (size_t i = 0; i < memgr.ram.size(); ++i) {
    auto row = i % ramTable->rowCount();
    auto column = i / ramTable->rowCount();
    auto pid = memgr.ram[i].first;
    auto vpage = !pid ? 0 : memgr.ram[i].second;

    if (row < ramTable->rowCount() && column < ramTable->columnCount()) {
      auto color = !pid ? Qt::gray : colorTable[pid % colorTableSize];
      ramTable->item(row, column)->setText(QString::asprintf("%.2d", vpage));
      ramTable->item(row, column)->setBackground(color);
    }
  }
}

void SimulationDialog::updateDisk() {
  const auto colorTableSize = sizeof(colorTable) / sizeof(*colorTable);

  for (int i = 0; i < diskTable->rowCount(); ++i) {
    for (int j = 0; j < diskTable->columnCount(); ++j) {
      diskTable->item(i, j)->setText("00");
      diskTable->item(i, j)->setBackground(Qt::gray);
    }
  }

  auto &memgr = sim->get_memory_manager();
  for (size_t i = 0; i < memgr.swap.size(); ++i) {
    auto row = i % diskTable->rowCount();
    auto column = i / diskTable->rowCount();
    auto pid = memgr.swap[i].first;
    auto vpage = !pid ? 0 : memgr.swap[i].second;

    if (row < diskTable->rowCount() && column < diskTable->columnCount()) {
      auto color = !pid ? Qt::gray : colorTable[pid % colorTableSize];
      diskTable->item(row, column)->setText(QString::asprintf("%.2d", vpage));
      diskTable->item(row, column)->setBackground(color);
    }
  }
}

void SimulationDialog::updatePageTable() {
  for (int i = 0; i < pageTable->rowCount(); ++i) {
    for (int j = 0; j < pageTable->columnCount(); ++j) {
      pageTable->item(i, j)->setBackground(Qt::white);
      pageTable->item(i, 0)->setData(Qt::DisplayRole, "");
      pageTable->item(i, 1)->setData(Qt::DisplayRole, "");
    }
  }

  auto &memgr = sim->get_memory_manager();
  for (size_t i = 0; i < memgr.page_table.size(); ++i) {
    auto ramPos = memgr.page_table[i].first;
    auto isValid = memgr.page_table[i].second;
    if (i < pageTable->rowCount()) {
      for (int j = 0; j < pageTable->columnCount(); ++j) {
        auto color = ramPos == size_t(-1) ? Qt::gray : Qt::white;
        pageTable->item(i, j)->setBackground(color);
      }

      QString ramPosString, isValidString;
      if (ramPos != size_t(-1)) {
        ramPosString = QString::number(ramPos);
        isValidString = isValid ? "Y" : "N";
      }

      pageTable->item(i, 0)->setData(Qt::DisplayRole, ramPosString);
      pageTable->item(i, 1)->setData(Qt::DisplayRole, isValidString);
    }
  }
}
