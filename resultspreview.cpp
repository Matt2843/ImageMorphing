#include "resultspreview.h"

#include "console.h"
#include "imagecontainer.h"

#include <QFileDialog>
#include <QDebug>

ResultsPreview::ResultsPreview(QWidget * parent) :
    ScrollableQGroupBox(parent, "Results Preview", ScrollableQGroupBox::Vertical)
{

}

void ResultsPreview::addMorphResult(ImageContainer *image)
{
    ImageContainer *container = new ImageContainer(this);
    container->setFixedSize(width(), height() / 3);
    container->update(image);
    m_container.insert(container);
    updatePreview();
    emit resultsNotEmpty();
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
        result->getSource().save(path);
    }
}
