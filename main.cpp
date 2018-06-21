#include "mainwindow.h"
#include <QApplication>

#include "commandlinemorphing.h"
#include "globals.h"

#include <QCommandLineParser>
#include <QCommandLineOption>

#include <memory>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("fmg-qt");
    QApplication::setApplicationVersion("1.0");
    std::unique_ptr<MainWindow> gui;

    QCommandLineParser parser;
    parser.setApplicationDescription("Face Morph Generation tool, to automate the morphing process of a directory of images.\n A json file can"
                                     "be added to specify post-processing effects such as brightness / contrast increase or gaussian / bilateral"
                                     "filtering.");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption guiOption(QStringList() << "gui",
                                 "Graphical User Interface");
    parser.addOption(guiOption);

    QCommandLineOption inputDirectoryOption(QStringList() << "i" << "input-directory",
                                            "Specifies the directory in which the images to be morphed resides",
                                            "directory");
    parser.addOption(inputDirectoryOption);

    QCommandLineOption outputDirectoryOption(QStringList() << "o" << "output-directory",
                                             "Specifies the target directory for the morphed results",
                                             "directory");
    parser.addOption(outputDirectoryOption);

    QCommandLineOption settingsOption(QStringList() << "s" << "settings",
                                      "Specifies the json-formatted settings file",
                                      "file");
    parser.addOption(settingsOption);

    parser.process(app);
    if(parser.isSet(guiOption) || argc == 1) {
        fmg::Globals::gui = true;
        gui = std::make_unique<MainWindow>(nullptr);
        gui->setStyleSheet("QMainWindow {background: 'white';}");
        gui->show();
    } else if(parser.isSet(inputDirectoryOption) && parser.isSet(outputDirectoryOption) && !parser.isSet(settingsOption)) { // default morphing procedure
        CommandLineMorphing(parser.value(inputDirectoryOption), parser.value(outputDirectoryOption));
    } else if(parser.isSet(inputDirectoryOption) && parser.isSet(outputDirectoryOption) && parser.isSet(settingsOption)) { // settings procedure
        CommandLineMorphing(parser.value(inputDirectoryOption), parser.value(outputDirectoryOption), parser.value(settingsOption));
    } else {
        parser.showHelp(1);
    }
    return app.exec();
}
