#pragma once

#include <QMainWindow>

#include "databasepreview.h"
#include "imageeditor.h"
#include "editorpane.h"
#include "resultspreview.h"

#include <memory>

#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QMainWindow * parent = nullptr);
    ~MainWindow() = default;

private:
    void setup();
    void setupConnections();

private:
    void createActions();
    void createMenus();

private slots:
    void newProject();
    void loadDatabaseFromDirectory();
    void loadDatabaseFromFiles();
    void exportResults();
    void exit();

    void createAutoScript();

    void about();

private:
    QAction *a_newProject;
    QAction *a_loadDatabaseFromDirectory;
    QAction *a_loadDatabaseFromFiles;
    QAction *a_exportResults;
    QAction *a_exit;
    QAction *a_createAutoScript;
    QAction *a_about;



private:
    std::unique_ptr<QWidget> m_content_pane;
    std::unique_ptr<QHBoxLayout> m_layout;
    std::unique_ptr<QVBoxLayout> m_second_layout;

private:
    std::unique_ptr<DatabasePreview> m_database_preview;
    std::unique_ptr<ImageEditor> m_image_editor;
    std::unique_ptr<ResultsPreview> m_results_preview;

private:
    QMenu *file_menu;
    QMenu *edit_menu;
    QMenu *help_menu;
};
