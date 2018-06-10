#include "mainwindow.h"

#include <QDebug>
#include <QMenuBar>
#include <QScreen>
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
    // make this relative.
    resize(screen->geometry().width() * 0.7, screen->geometry().height() * 0.7);
    // set central widget and initialize the layout.
    setCentralWidget(m_content_pane);
    m_content_pane->setLayout(m_layout);

    // Add the database preview
    m_layout->addWidget(m_database_preview, 1);

    // Add the editor and results preview
    m_second_layout->addWidget(m_image_editor, 3);
    m_second_layout->addWidget(m_results_preview, 1);

    m_layout->addLayout(m_second_layout, 3);
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

    a_createAutoScript = new QAction(tr("Create Script"), this);
    a_createAutoScript->setStatusTip("Automate the morphing process");
    connect(a_createAutoScript, &QAction::triggered, this, &MainWindow::createAutoScript);

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

    edit_menu = menuBar()->addMenu(tr("&Edit"));
    edit_menu->addAction(a_createAutoScript);

    help_menu = menuBar()->addMenu(tr("&Help"));
    help_menu->addAction(a_about);
}

void MainWindow::newProject()
{
    qDebug() << "new project pressed";
}

void MainWindow::loadDatabaseFromDirectory()
{
    qDebug() << "load database from directory pressed";
    m_database_preview->loadDatabaseFromDirectory();
}

void MainWindow::loadDatabaseFromFiles()
{
    qDebug() << "load database from files pressed";
    m_database_preview->loadDatabaseFromFiles();
}

void MainWindow::exportResults()
{
    qDebug() << "export results pressed";
}

void MainWindow::exit()
{
    qDebug() << "exit pressed";
}

void MainWindow::createAutoScript()
{
    qDebug() << "create script pressed";
}

void MainWindow::about()
{
    qDebug() << "about pressed";
}
