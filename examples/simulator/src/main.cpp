#include "simulatorgui.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  SimulatorGUI window;
  window.show();
  return app.exec();
}
