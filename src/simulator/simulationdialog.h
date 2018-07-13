#ifndef SIMULATIONDIALOG_H
#define SIMULATIONDIALOG_H

#include <QDialog>
#include <sosim/simulator.hpp>

namespace Ui {
class SimulationDialog;
}

class SimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationDialog(std::unique_ptr<sosim::Simulator> sim,
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
    int timer;

    Ui::SimulationDialog *ui;
};

#endif // SIMULATIONDIALOG_H
