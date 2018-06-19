#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>
#include <QDebug>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QApplication::setApplicationName("fmg-qt");
    QApplication::setApplicationVersion("1.0");

    QCommandLineParser parser;
    parser.setApplicationDescription(QApplication::translate("main",
                                                             "Face morph generator tool"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption inputDirectoryOption(QStringList() << "I" << "input-directory",
                                            QApplication::translate("main", "Specifies the directory in which the images to be morphed resides."),
                                            QApplication::translate("main", "directory"));

    parser.addOption(inputDirectoryOption);

    parser.process(a);
    parser.showHelp(1);
    return a.exec();
}
