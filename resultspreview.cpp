#include "resultspreview.h"

#include "console.h"
#include "imagecontainer.h"

#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMouseEvent>
#include <QPushButton>
#include <QDialog>

/**
 * @brief ResultsPreview::ResultsPreview
 *
 * The default ResultsPreview ctor, constructing the parent ScrollableQGroupBox.
 *
 * @param parent the Qt parent widget
 */
ResultsPreview::ResultsPreview(QWidget * parent) :
    ScrollableQGroupBox(parent, "Results Preview", ScrollableQGroupBox::Vertical) {}

/**
 * @brief ResultsPreview::addMorphResult
 *
 * A public SLOT to add a finished morph result to the ResultsPreview view.
 *
 * @param image the image to be added to the view
 */
void ResultsPreview::addMorphResult(ImageContainer *image)
{
    ImageContainer *container = new ImageContainer(this);
    connect(container, SIGNAL(mousePressDetected(ImageContainer*, QMouseEvent*)),
            this, SLOT(removeMorphResult(ImageContainer*, QMouseEvent*)));
    container->setFixedSize(width() - width() / 4.6, height() / 3);
    container->update(image);
    m_container.insert(container);
    updatePreview();
    Console::appendToConsole("Added result: " + container->getImageTitle());
    emit resultsNotEmpty();
}

/**
 * @brief ResultsPreview::removeMorphResult
 *
 * A public SLOT invoked when the user right-clicks a ImageContainer in the ResultsPreview view.
 *
 * @param img the image to be removed from the view
 * @param event the QMouseEvent used for tooltip positioning
 */
void ResultsPreview::removeMorphResult(ImageContainer *img, QMouseEvent *event)
{
    QMenu *popup = new QMenu(this);
    QAction *delete_morph = new QAction("Remove", this);
    popup->addAction(delete_morph);

    QAction *action = popup->exec(event->globalPos());

    if(action == delete_morph) {
        Console::appendToConsole("Removed result: " + img->getImageTitle());
        m_container.erase(img);
        delete img;
    }
}

/**
 * @brief ResultsPreview::exportResults
 *
 * The procedure invoked when the "Export Results" menu action is invoked by the application-user.
 * The procedure exports all contained results to a directory selected by the application-user and
 * saves the results according to the const char *format input parameter.
 *
 */
void ResultsPreview::exportResults()
{
    auto directory_path = QFileDialog::getExistingDirectory(this,
                                                            tr("Select Database"),
                                                            "",
                                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDialog jpg_png_diag;
    const char *format = "jpg";

    QVBoxLayout o_layout;
    QHBoxLayout i_layout;
    QLabel choice("Please choose a format:");
    QPushButton png("png");
    QPushButton jpg("jpg");
    i_layout.addWidget(&png);
    i_layout.addWidget(&jpg);
    o_layout.addWidget(&choice);
    o_layout.addLayout(&i_layout);
    jpg_png_diag.setLayout(&o_layout);
    connect(&png, &QPushButton::released, [&format](){format = "png";});
    connect(&jpg, &QPushButton::released, [&format](){format = "jpg";});
    connect(&png, &QPushButton::released, &jpg_png_diag, &QDialog::close);
    connect(&jpg, &QPushButton::released, &jpg_png_diag, &QDialog::close);
    jpg_png_diag.exec();
    for(auto &result : m_container) {
        QString path = directory_path + "/" + result->getImageTitle() + "_" + result->getId() + "." + format;
        Console::appendToConsole("Saving: " + path);
        result->getTempSource().save(path);
    }
}
