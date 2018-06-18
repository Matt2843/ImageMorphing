#include "mainwindow.h"

#include "morphdatabasedialog.h"
#include "databasepreview.h"
#include "imageeditor.h"
#include "resultspreview.h"

#include <QDebug>
#include <QMenuBar>
#include <QScreen>
#include <QApplication>
#include <QGuiApplication>

/**
 * @brief MainWindow::MainWindow
 * Default constructor of the MainWindow class
 * initialises a content pane widget and a
 * vertical box layout.
 * @param parent
 */
MainWindow::MainWindow(QMainWindow *parent) :
    QMainWindow(parent),
    m_content_pane(new QWidget(this)),
    m_layout(new QHBoxLayout()),
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
 * An auxillary private method to
 * set up the main-window internal properties
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
 * An auxillary private method to set-up the internal
 * Qt conneciton properties.
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
 * An auxillary private mathod to initialize the menu actions.
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

void MainWindow::newProject()
{
    m_database_preview->clearContainerAndPreview();
    m_results_preview->clearContainerAndPreview();
    m_image_editor->resetAll();
}

void MainWindow::loadDatabaseFromDirectory()
{
    m_database_preview->loadDatabaseFromDirectory();
}

void MainWindow::loadDatabaseFromFiles()
{
    m_database_preview->loadDatabaseFromFiles();
}

void MainWindow::exportResults()
{
    m_results_preview->exportResults("jpg");
}

void MainWindow::exit()
{
    QApplication::instance()->quit();
}

void MainWindow::generateMorphDatabase()
{
    MorphDatabaseDialog d;
    d.exec();
}

void MainWindow::about()
{
    qDebug() << "about pressed";
}
