#include "imagecontainer.h"

#include <QRegExp>

/**
 * @brief ImageContainer::ImageContainer
 * The default ImageContainer constructor
 * @param parent
 */
ImageContainer::ImageContainer(QWidget *parent) : QLabel(parent),
    m_source(std::make_unique<QImage>(nullptr))
{
    setScaledContents(true);
    setFrameShape(QFrame::Box);
    setLineWidth(1);
}

/**
 * @brief ImageContainer::updatePixmap
 * A public slot to update the image-container pixmap
 * when the target image pointer has changed.
 * @param image
 */
void ImageContainer::updatePixmap(QImage *image)
{
    setPixmap(QPixmap::fromImage(*image));
}

/**
 * @brief ImageContainer::setImageSource
 * Set the pixmap of this container to the image in the input path.
 * @param path
 * @return true if succesful
 */
bool ImageContainer::setImageSource(const QString &path)
{
    auto loaded = m_source->load(path);
    if(!loaded) return false;
    m_img_path = path;
    m_img_title = m_img_path.toString();
    m_img_title.replace(QRegExp("(.jpg)|(.png)|(.jpeg)"),"");
    m_img_title.replace(QRegExp(".*/"),"");
    setPixmap(QPixmap::fromImage(*m_source));
    emit sourceChanged(m_source.get());
    return true;
}

/**
 * @brief ImageContainer::getSource
 * @return
 */
QImage * ImageContainer::getSource()
{
    return m_source.get();
}

/**
 * @brief ImageContainer::getImagePath
 * @return
 */
QUrl ImageContainer::getImagePath()
{
   return m_img_path;
}

/**
 * @brief ImageContainer::getImageTitle
 * @return
 */
QString ImageContainer::getImageTitle()
{
    return m_img_title;
}
