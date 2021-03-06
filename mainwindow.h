#pragma once
#include <QMainWindow>

class QWidget;
class QMenu;
class QAction;
class DatabasePreview;
class ImageEditor;
class ResultsPreview;
class QHBoxLayout;
class QVBoxLayout;
class MainWindow : public QMainWindow
{
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

    void generateMorphDatabase();

    void about();

private:
    QAction *a_newProject;
    QAction *a_loadDatabaseFromDirectory;
    QAction *a_loadDatabaseFromFiles;
    QAction *a_exportResults;
    QAction *a_exit;
    QAction *a_generateMorphDatabase;
    QAction *a_about;

private:
    QWidget *m_content_pane;
    QHBoxLayout *m_layout;
    QVBoxLayout *m_second_layout;

private:
    DatabasePreview *m_database_preview;
    ImageEditor *m_image_editor;
    ResultsPreview *m_results_preview;

private:
    QMenu *file_menu;
    QMenu *edit_menu;
    QMenu *help_menu;
};
