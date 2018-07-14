#ifndef SIMULATIONDIALOG_H
#define SIMULATIONDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>
#include <sosim/simulator.hpp>

namespace Ui {
class SimulationDialog;
}

class SimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationDialog(std::unique_ptr<sosim::Simulator> sim,
                              unsigned simulationTimeStep,
                              QWidget *parent = 0);
    ~SimulationDialog();

    virtual int exec();

    void performTick();
    void updateScreen();
    void updateTimeline();
    void updateRAM();
    void updateDisk();
    void updatePageTable();

protected:
    virtual void timerEvent(QTimerEvent* event);

private:
    std::unique_ptr<sosim::Simulator> sim;
    unsigned simulationTimeStep;
    int timerHandle;

    QLabel* responseLabel;
    QTableWidget* timelineTable;
    QTableWidget* ramTable;
    QTableWidget* diskTable;
    QTableWidget* pageTable;

    Ui::SimulationDialog *ui;
};

#endif // SIMULATIONDIALOG_H
