#include "databasepreview.h"

#include <numeric>

#include <QString>
#include <QDebug>
#include <QFileDialog>
#include <QDirIterator>
#include <QApplication>

#include <QMenu>
#include <QAction>

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

int DatabasePreview::m_image_width = 0;
int DatabasePreview::m_image_height = 0;

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
 * @brief imageResolutionDialog
 * @return
 */
bool imageResolutionDialog()
{
    bool result = false;
    QDialog selectImageResolutionDialog;
    QVBoxLayout diag_layout;
    QHBoxLayout diag_group_one;
    QHBoxLayout diag_group_two;
    QHBoxLayout diag_group_three;
    QLabel d_title("Please select an image-resolution");
    d_title.setAlignment(Qt::AlignCenter);
    QLabel d_img_w_lab("Width: ");
    QLabel d_img_h_lab("Height: ");
    QLineEdit d_w_e;
    d_w_e.setText(QString::number(DatabasePreview::m_image_width));
    QLineEdit d_h_e;
    d_h_e.setText(QString::number(DatabasePreview::m_image_height));
    QPushButton d_accept("Ok");
    QPushButton d_cancel("Cancel");
    diag_layout.addWidget(&d_title);
    diag_group_one.addWidget(&d_img_w_lab,1);
    diag_group_one.addWidget(&d_w_e,2);
    diag_group_two.addWidget(&d_img_h_lab,1);
    diag_group_two.addWidget(&d_h_e,2);
    diag_layout.addLayout(&diag_group_one);
    diag_layout.addLayout(&diag_group_two);
    diag_group_three.addWidget(&d_accept);
    diag_group_three.addWidget(&d_cancel);
    diag_layout.addLayout(&diag_group_three);
    QObject::connect(&d_accept, &QPushButton::released, [&result, &d_w_e, &d_h_e, &selectImageResolutionDialog](){
        int width = d_w_e.text().toInt();
        int height = d_h_e.text().toInt();
        if(width > 0 && height > 0) {
            result = true;
            DatabasePreview::m_image_width = width;
            DatabasePreview::m_image_height = height;
            selectImageResolutionDialog.close();
        }
    });
    QObject::connect(&d_cancel, &QPushButton::released, [&selectImageResolutionDialog](){
        selectImageResolutionDialog.close();
    });
    selectImageResolutionDialog.setLayout(&diag_layout);
    selectImageResolutionDialog.setModal(true);
    selectImageResolutionDialog.exec();
    return result;
}

void scanImageResolutions(const QStringList &image_file_paths)
{
    std::vector<int> widths;
    std::vector<int> heights;
    for(const QString & path : image_file_paths) {
        QImage img(path);
        widths.push_back(img.width());
        heights.push_back(img.height());
    }
    DatabasePreview::m_image_width = (int) (std::accumulate(widths.begin(), widths.end(), 0) / widths.size());
    DatabasePreview::m_image_height = (int) (std::accumulate(heights.begin(), heights.end(), 0) / heights.size());
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
    scanImageResolutions(image_file_paths);
    if(!imageResolutionDialog()) return false;
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
