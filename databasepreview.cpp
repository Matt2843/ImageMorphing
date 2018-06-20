#include "databasepreview.h"

#include "console.h"
#include "globals.h"

#include <algorithm>

#include <QString>
#include <QFileDialog>
#include <QDirIterator>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMouseEvent>

/**
 * @brief DatabasePreview::DatabasePreview
 *
 * The DatabasePreview ctor initializing the parent
 * container if parent != nullptr.
 *
 * @param parent the Qt widgets parent of this widget.
 */
DatabasePreview::DatabasePreview(QWidget * parent) :
    ScrollableQGroupBox(parent, "Database Preview") {}

/**
 * @brief DatabasePreview::loadDatabaseFromFiles
 *
 * A procedure to allow the application-user to select a set of images to be loaded into the application.
 *
 * @return true if success
 */
bool DatabasePreview::loadDatabaseFromFiles()
{
    clearContainerAndPreview();
    QStringList image_file_paths = QFileDialog::getOpenFileNames(this,
                                                                 tr("Load Images"),
                                                                 QDir::currentPath(),
                                                                 tr("*.jpg *.png *.jpeg"));
    if(image_file_paths.isEmpty()) return false;
    if(!loadDatabase(image_file_paths)) return false;
    updatePreview();
    return true;
}

/**
 * @brief DatabasePreview::loadDatabaseFromDirectory
 *
 * A procedure to allow the application-user to select a database to be loaded into the application.
 *
 * @return true if success
 */
bool DatabasePreview::loadDatabaseFromDirectory()
{
    clearContainerAndPreview();
    QString directory_path = QFileDialog::getExistingDirectory(this,
                                                               tr("Select Database"),
                                                               QDir::currentPath(),
                                                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(directory_path.isEmpty()) return false;
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
 * @brief DatabasePreview::imageRightClickInvoked
 *
 * A private SLOT invoked when the application-user right clicks a image in the
 * DatabasePreview container. The method is invoked when a mouse-click
 * SIGNAL has been emitted.
 *
 * @param img the image right-clicked.
 * @param event used for tooltip positioning.
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

/**
 * @brief DatabasePreview::loadDatabase
 *
 * An private class convenience method invoked by loadDatabaseFromDirectory or
 * loadDatabaseFromFiles. The method initializes and adds images contained in the
 * application-user specified input directory to the DatabasePreview view.
 *
 * @param image_file_paths a list of the detected images in the application-user specified directory.
 * @return true if everything went accordingly
 */
bool DatabasePreview::loadDatabase(const QStringList &image_file_paths)
{
    scanImageResolutions(image_file_paths);
    if(!imageResolutionDialog()) return false;
    for(const QString & path : image_file_paths) {
        Console::appendToConsole("Adding and scaling: " + path);
        ImageContainer *image = new ImageContainer(this);
        image->setFixedSize(width() - width() / 4.6, height() / 3);
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
 * @brief DatabasePreview::scanImageResolutions
 *
 * An auxially routine to scan images for their resolution and set the global
 * fmg::Globals::img_width and fmg::Globals::img_height. These global variables
 * are used throughout the application when determining the resolution of the
 * generated outputs, as well as the scaling of the inputs.
 *
 * @param image_file_paths
 */
void DatabasePreview::scanImageResolutions(const QStringList &image_file_paths)
{
    std::vector<int> widths;
    std::vector<int> heights;
    for(const QString & path : image_file_paths) {
        QImage img(path);
        widths.push_back(img.width());
        heights.push_back(img.height());
    }
    fmg::Globals::img_width = (int)(*std::min_element(widths.begin(), widths.end()));
    fmg::Globals::img_height = (int)(*std::min_element(heights.begin(), heights.end()));
    Console::appendToConsole("Minimum resolution: " + QString::number(fmg::Globals::img_width)
                             + "x" + QString::number(fmg::Globals::img_height) + " suggested to user.");
}

/**
 * @brief imageResolutionDialog
 *
 * A convenience private class method to allow the application-user to
 * redefine the auto-detected image scalar. The method creates a modal
 * dialog which requires application-user interaction, this method is invoked
 * whenever a valid directory/set of files has been marked for DatabasePreview loading.
 *
 * @return
 */
bool DatabasePreview::imageResolutionDialog()
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
    d_w_e.setText(QString::number(fmg::Globals::img_width));
    QLineEdit d_h_e;
    d_h_e.setText(QString::number(fmg::Globals::img_height));
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
            fmg::Globals::img_width = width;
            fmg::Globals::img_height = height;
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
