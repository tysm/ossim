#ifndef SIMULATORGUI_H
#define SIMULATORGUI_H

#include <QMainWindow>
#include <sosim/simulator.hpp>
using namespace sosim;

namespace Ui {
class SimulatorGUI;
}

class SimulatorGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorGUI(QWidget *parent = 0);
    ~SimulatorGUI();

private slots:

    void on_Next_clicked();


private:

    std::unique_ptr<Simulator> sosim;

    Ui::SimulatorGUI *ui;
};

#endif // SIMULATORGUI_H
