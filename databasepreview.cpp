#include "databasepreview.h"

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QDirIterator>
#include <QApplication>

#include <QMenu>
#include <QAction>

/**
 * @brief DatabasePreview::DatabasePreview
 * The default DatabasePreview constructor
 */
DatabasePreview::DatabasePreview(QWidget * parent) :
    ScrollableQGroupBox(parent, "Database Preview") {}

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
        ImageContainer *image = new ImageContainer(this);
        image->resize(m_content_pane->geometry().width(), m_content_pane->geometry().height() / 5);
        if(!image->setImageSource(path)) return false;

        connect(image, SIGNAL(doubleClickDetected(ImageContainer*)),
                this, SIGNAL(imageDoubleClicked(ImageContainer*)));
        connect(image, SIGNAL(mousePressDetected(ImageContainer*, QMouseEvent*)),
                this, SLOT(imageRightClickInvoked(ImageContainer*, QMouseEvent*)));

        m_container.insert(image);
    }
    return true;
}

/**
 * @brief DatabasePreview::imageRightClickInvoked
 * @param img
 * @param event
 */
void DatabasePreview::imageRightClickInvoked(ImageContainer *img, QMouseEvent *event)
{
    //TODO: create a menu at the cursor loc. (event->globalPos())
    QMenu *popup = new QMenu(this);

    QAction *refOneAction = new QAction("Set as Reference One", this);
    QAction *refTwoAction = new QAction("Set as Reference Two", this);

    popup->addAction(refOneAction);
    popup->addAction(refTwoAction);

    QAction *action = popup->exec(event->globalPos());

    if(action == refOneAction) {
        emit referenceImageRequest(img, 1);
    } else if(action == refTwoAction) {
        emit referenceImageRequest(img, 2);
    }
}
