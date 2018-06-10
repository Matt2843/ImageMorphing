#include "databasepreview.h"

#include <QString>
#include <QFileDialog>
#include <QDirIterator>

/**
 * @brief DatabasePreview::DatabasePreview
 * The default DatabasePreview constructor
 */
DatabasePreview::DatabasePreview() : ScrollableQGroupBox(nullptr, "Database Preview") {}

/**
 * @brief DatabasePreview::loadDatabaseFromFiles
 * A procedure to allow the application-user to
 * select a set of images to be loaded into the application.
 * @return true if success
 */
bool DatabasePreview::loadDatabaseFromFiles()
{
    clearContainerAndPreview();
    QStringList image_file_paths = QFileDialog::getOpenFileNames(this,
                                                                 tr("Load Images"),
                                                                 QDir::currentPath(),
                                                                 tr("*.jpg *.png *.jpeg"));
    if(!loadDatabase(image_file_paths)) return false;
    updatePreview();
    return true;
}

/**
 * @brief DatabasePreview::loadDatabaseFromDirectory
 * A procedure to allow the application-user to
 * select a database to be loaded into the application.
 * @return true if success
 */
bool DatabasePreview::loadDatabaseFromDirectory()
{
    clearContainerAndPreview();
    QString directory_path = QFileDialog::getExistingDirectory(this,
                                                               tr("Select Database"),
                                                               "/home",
                                                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QStringList image_file_paths;
    QDirIterator it(directory_path, QStringList() << "*.jpg" << "*.png" << "*.jpeg", QDir::Files);
    while(it.hasNext()) {
        image_file_paths << it.next();
    }
    if(!loadDatabase(image_file_paths)) return false;
    updatePreview();
    return true;
}

/**
 * @brief DatabasePreview::loadDatabase
 * An auxillary method to load images into the database-preview
 * as well as into the m_database container for house-keeping.
 * @param image_file_paths
 * @return true if everything went accordingly
 */
bool DatabasePreview::loadDatabase(const QStringList &image_file_paths)
{
    for(const QString & path : image_file_paths) {
        std::unique_ptr<ImageContainer> image = std::make_unique<ImageContainer>(nullptr);
        image->resize(m_content_pane->geometry().width(), m_content_pane->geometry().height() / 8);
        if(!image->setImageSource(path)) return false;
        m_container.insert(std::move(image));
    }
    return true;
}
