#include "mainwindow.h"
#include <QApplication>
#include "morphdatabasedialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setStyleSheet("QMainWindow {background: 'white';}");
    w.show();
    return a.exec();
}
