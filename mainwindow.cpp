#include "mainwindow.h"
#include <QDebug>

#include "morphdatabasedialog.h"
#include "databasepreview.h"
#include "imageeditor.h"
#include "editorpane.h"
#include "resultspreview.h"

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QScreen>
#include <QWidget>
#include <QDialog>
#include <QApplication>
#include <QGuiApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

/**
 * @brief MainWindow::MainWindow
 *
 * The MainWindow ctor, constructing the DatabasePreview,
 * ImageEditor and ResultsPreview.
 *
 * @param parent
 */
MainWindow::MainWindow(QMainWindow *parent) :
    QMainWindow(parent),
    m_content_pane(new QWidget(this)),
    m_layout(new QHBoxLayout),
    m_second_layout(new QVBoxLayout()),
    m_database_preview(new DatabasePreview(this)),
    m_image_editor(new ImageEditor(this)),
    m_results_preview(new ResultsPreview(this))
{
    createActions();
    createMenus();
    setup();
    setupConnections();
}

/**
 * @brief MainWindow::setup
 *
 * A private convenience method for setting up the layout and internal widgets of this container.
 *
 */
void MainWindow::setup()
{
    QScreen * screen = QGuiApplication::primaryScreen();
    resize(screen->geometry().width() * 0.7, screen->geometry().height() * 0.7);
    setCentralWidget(m_content_pane);
    m_content_pane->setLayout(m_layout);

    m_layout->addWidget(m_database_preview, 1);
    m_layout->addWidget(m_image_editor, 3);
    m_layout->addWidget(m_results_preview, 1);
}

/**
 * @brief MainWindow::setupConnections
 *
 * A private convenience method for setting up the Qt SLOTS/SIGNAL connections.
 *
 */
void MainWindow::setupConnections()
{
    connect(m_database_preview, SIGNAL(imageDoubleClicked(ImageContainer*)),
            m_image_editor, SLOT(attemptToSetReferenceByDoubleClick(ImageContainer*)));

    connect(m_database_preview, SIGNAL(referenceImageRequest(ImageContainer*,int)),
            m_image_editor, SLOT(attemptToSetReference(ImageContainer*,int)));

    connect(m_image_editor->getEditorPane(), SIGNAL(addToResultsInvoked(ImageContainer*)),
            m_results_preview, SLOT(addMorphResult(ImageContainer*)));

    connect(m_results_preview, &ResultsPreview::resultsNotEmpty, [&](){
        a_exportResults->setEnabled(true);
    });
}

/**
 * @brief MainWindow::createActions
 *
 * An private class method to initialize the various menu actions.
 *
 */
void MainWindow::createActions()
{
    a_newProject = new QAction(tr("New Project"), this);
    a_newProject->setShortcut(QKeySequence::New);
    a_newProject->setStatusTip(tr("Create a new project"));
    connect(a_newProject, &QAction::triggered, this, &MainWindow::newProject);

    a_loadDatabaseFromDirectory = new QAction(tr("Load Database (dir)"), this);
    a_loadDatabaseFromDirectory->setShortcut(QKeySequence::Open);
    a_loadDatabaseFromDirectory->setStatusTip(tr("Load a directory of images"));
    connect(a_loadDatabaseFromDirectory, &QAction::triggered, this, &MainWindow::loadDatabaseFromDirectory);

    a_loadDatabaseFromFiles = new QAction(tr("Load Database (files)"), this);
    a_loadDatabaseFromFiles->setStatusTip(tr("Load image files"));
    connect(a_loadDatabaseFromFiles, &QAction::triggered, this, &MainWindow::loadDatabaseFromFiles);

    a_exportResults = new QAction(tr("Export Results"), this);
    a_exportResults->setShortcut(QKeySequence::SaveAs);
    a_exportResults->setStatusTip("Export the generated images");
    a_exportResults->setEnabled(false);
    connect(a_exportResults, &QAction::triggered, this, &MainWindow::exportResults);

    a_exit = new QAction(tr("Exit"), this);
    a_exit->setShortcut(QKeySequence::Close);
    a_exit->setStatusTip("Exit");
    connect(a_exit, &QAction::triggered, this, &MainWindow::exit);

    a_generateMorphDatabase = new QAction(tr("Generate Morph-Database"), this);
    a_generateMorphDatabase->setStatusTip("Automate the morphing process");
    connect(a_generateMorphDatabase, &QAction::triggered, this, &MainWindow::generateMorphDatabase);

    a_about = new QAction(tr("About"), this);
    connect(a_about, &QAction::triggered, this, &MainWindow::about);
}

/**
 * @brief MainWindow::createMenus
 *
 * A private class method to create the menus contained in the menubar
 *
 */
void MainWindow::createMenus()
{
    file_menu = menuBar()->addMenu(tr("&File"));
    file_menu->addAction(a_newProject);
    file_menu->addSeparator();
    file_menu->addAction(a_loadDatabaseFromDirectory);
    file_menu->addAction(a_loadDatabaseFromFiles);
    file_menu->addSeparator();
    file_menu->addAction(a_exportResults);
    file_menu->addSeparator();
    file_menu->addAction(a_exit);

    edit_menu = menuBar()->addMenu(tr("&Tools"));
    edit_menu->addAction(a_generateMorphDatabase);

    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(a_about);
}

/**
 * @brief MainWindow::newProject
 *
 * The SLOT invoked when the application-user activates the new project menu action.
 *
 */
void MainWindow::newProject()
{
    m_database_preview->clearContainerAndPreview();
    m_results_preview->clearContainerAndPreview();
    m_image_editor->resetAll();
    a_exportResults->setEnabled(false);
}

/**
 * @brief MainWindow::loadDatabaseFromDirectory
 *
 * The SLOT invoked when the application-user activates load database (dir) menu action.
 *
 */
void MainWindow::loadDatabaseFromDirectory()
{
    m_database_preview->loadDatabaseFromDirectory();
}

/**
 * @brief MainWindow::loadDatabaseFromFiles
 *
 * The SLOT invoked when the application-user activates the load database (files) menu action.
 *
 */
void MainWindow::loadDatabaseFromFiles()
{
    m_database_preview->loadDatabaseFromFiles();
}

/**
 * @brief MainWindow::exportResults
 *
 * The SLOT invoked when the application-user activates the export results menu action.
 *
 */
void MainWindow::exportResults()
{
    m_results_preview->exportResults();
}

/**
 * @brief MainWindow::exit
 *
 * The SLOT invoked when the application-user activates the exit menu action.
 *
 */
void MainWindow::exit()
{
    QApplication::instance()->quit();
}

/**
 * @brief MainWindow::generateMorphDatabase
 *
 * The SLOT invoked when the application-user activates the generate morph database menu action.
 *
 */
void MainWindow::generateMorphDatabase()
{
    MorphDatabaseDialog d;
    d.exec();
}

/**
 * @brief MainWindow::about
 *
 * The SLOT invoked when the application-user activates the about menu action.
 *
 */
void MainWindow::about()
{
    QDialog about;
    QVBoxLayout layout;
    QLabel text;
    text.setText("A Face Morph Generator created as a part of the\n"
                 "02238 - Biometric Systems course at DTU, the fmg-qt\n"
                 "application is on a proof-of-concept basis.\n\n"
                 "Author: Mathias Henriksen s134532@student.dtu.dk");
    layout.addWidget(&text);
    QPushButton ok("Ok");
    layout.addWidget(&ok);
    connect(&ok, &QPushButton::released, &about, &QDialog::close);
    about.setLayout(&layout);
    about.exec();
}
