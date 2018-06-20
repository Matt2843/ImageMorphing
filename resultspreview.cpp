#include "resultspreview.h"

#include "console.h"
#include "imagecontainer.h"

#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QMouseEvent>

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
 * @param format ".png" or ".jpg"
 */
void ResultsPreview::exportResults(const char *format)
{
    auto directory_path = QFileDialog::getExistingDirectory(this,
                                                            tr("Select Database"),
                                                            "",
                                                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    for(auto &result : m_container) {
        QString path = directory_path + "/" + result->getImageTitle() + "_" + result->getId() + "." + format;
        Console::appendToConsole("Saving: " + path);
        result->getTempSource().save(path);
    }
}
