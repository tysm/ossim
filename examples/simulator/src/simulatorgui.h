#ifndef SIMULATORGUI_H
#define SIMULATORGUI_H

#include <QMainWindow>

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
    void on_btn_next_released();

private:
    Ui::SimulatorGUI *ui;
};

#endif // SIMULATORGUI_H
