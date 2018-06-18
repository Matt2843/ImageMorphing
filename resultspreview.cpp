#include "resultspreview.h"

#include "console.h"
#include "imagecontainer.h"

#include <QFileDialog>
#include <QDebug>

#include <QMenu>
#include <QAction>

ResultsPreview::ResultsPreview(QWidget * parent) :
    ScrollableQGroupBox(parent, "Results Preview", ScrollableQGroupBox::Vertical)
{

}

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
