#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

void parse(int argc, char *argv[])
{
    QCommandLineParser parser;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("image-morphing");
    QApplication::setApplicationVersion("1.0");
    if(argc == 1) {
        MainWindow w;
        w.setStyleSheet("QMainWindow {background: 'white';}");
        w.show();
    } else {
        parse(argc, argv);
    }

    MainWindow w;
    w.setStyleSheet("QMainWindow {background: 'white';}");
    w.show();
    return a.exec();
}
