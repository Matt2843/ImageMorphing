#include "imagecontainer.h"

#include "databasepreview.h"

#include <QDebug>
#include <QRegExp>
#include <QMouseEvent>

#include <QBitmap>
#include <QPainter>
#include <QPen>

/**
 * @brief ImageContainer::ImageContainer
 * The default ImageContainer constructor
 * @param parent
 */
ImageContainer::ImageContainer(QWidget *parent) :
    QLabel(parent),
    m_contains_image(false)
{
    setScaledContents(true);
    setFrameShape(QFrame::Box);
    setLineWidth(1);
}

/**
 * @brief ImageContainer::mouseDoubleClickEvent
 * @param event
 */
void ImageContainer::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClickDetected(this);
}

/**
 * @brief ImageContainer::mousePressEvent
 */
void ImageContainer::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        emit mousePressDetected(this, event);
}

/**
 * @brief ImageContainer::update
 * @param other
 */
void ImageContainer::update(ImageContainer *other)
{
    m_img_path = other->m_img_path;
    m_img_title = other->m_img_title;
    m_source = other->m_source;
    m_contains_image = other->m_contains_image;
    m_landmark_image = other->m_landmark_image;
    m_landmarks = other->m_landmarks;
    m_isDisplayingLandmarks = other->m_isDisplayingLandmarks;
    setImage(m_source);
}

/**
 * @brief ImageContainer::setImageSource
 * Set the pixmap of this container to the image in the input path.
 * @param path
 * @return true if succesful
 */
bool ImageContainer::setImageSource(const QString &path)
{
    auto loaded = m_source.load(path);
    if(!loaded) return false;
    m_source = m_source.scaled(DatabasePreview::m_image_width, DatabasePreview::m_image_height);
    m_contains_image = true;
    m_img_path = path;
    m_img_title = m_img_path.toString();
    m_img_title.replace(QRegExp("(.jpg)|(.png)|(.jpeg)"),"");
    m_img_title.replace(QRegExp(".*/"),"");
    setPixmap(QPixmap::fromImage(m_source));
    return true;
}

/**
 * @brief ImageContainer::setImageSource
 * @param source
 */
void ImageContainer::setImageSource(const QImage &source)
{
    m_source = source.copy();
    m_contains_image = true;
    setPixmap(QPixmap::fromImage(m_source));
}

/**
 * @brief ImageContainer::setTempImage
 * @param image
 */
void ImageContainer::setImage(const QImage &image)
{
    m_contains_image = true;
    setPixmap(QPixmap::fromImage(image));
}

/**
 * @brief ImageContainer::getSource
 * @return
 */
QImage ImageContainer::getSource()
{
    return m_source.copy();
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

/**
 * @brief ImageContainer::hasImage
 * @return
 */
bool ImageContainer::hasImage()
{
   return m_contains_image;
}

/**
 * @brief ImageContainer::getLandmarks
 * @return
 */
std::vector<QPoint> ImageContainer::getLandmarks()
{
    return m_landmarks;
}

/**
 * @brief ImageContainer::setLandmarks
 * @param landmarks
 */
void ImageContainer::setLandmarks(const std::vector<QPoint> &landmarks)
{
    m_landmarks = landmarks;

    // manually add corner points
    m_landmarks.push_back(QPoint(0, 0)); // top-left
    m_landmarks.push_back(QPoint(m_source.width() - 1, 0)); // top-right
    m_landmarks.push_back(QPoint(0, m_source.height() - 1)); // bot-left
    m_landmarks.push_back(QPoint(m_source.width() - 1, m_source.height() - 1)); // bot-right

    // manually add midpoints
    m_landmarks.push_back(QPoint(0, m_source.height() / 2)); // mid-left
    m_landmarks.push_back(QPoint(m_source.width() / 2, 0)); // mid-top
    m_landmarks.push_back(QPoint(m_source.width() - 1, m_source.height() / 2)); // mid-right
    m_landmarks.push_back(QPoint(m_source.width() / 2, m_source.height() - 1)); // mid-bot

    generateLandmarkImage();
}

/**
 * @brief ImageContainer::hasLandmarks
 * @return
 */
bool ImageContainer::hasLandmarks()
{
    return !m_landmarks.empty();
}

/**
 * @brief ImageContainer::toggleLandmarks
 */
void ImageContainer::toggleLandmarks()
{
    if(m_isDisplayingLandmarks) {
        displayOriginal();
    } else {
        displayLandmarks();
    }
}

/**
 * @brief ImageContainer::generateLandmarkImage
 */
void ImageContainer::generateLandmarkImage()
{
    QImage res(m_source.size(), QImage::Format_ARGB32);
    QPainter painter(&res);
    QPen pen;
    pen.setColor("#d8d788");
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidth(4);
    painter.setPen(pen);
    painter.drawImage(QRect(0, 0, res.width(), res.height()), m_source, QRect(0, 0, res.width(), res.height()));
    for(const QPoint & landmark : m_landmarks) {
        painter.drawPoint(landmark.x(), landmark.y());
    }
    m_landmark_image = res;
}

void ImageContainer::displayOriginal()
{
    if(m_source.isNull()) return;
    setPixmap(QPixmap::fromImage(m_source));
    m_isDisplayingLandmarks = false;
}

void ImageContainer::displayLandmarks()
{
    if(m_landmark_image.isNull()) return;
    setPixmap(QPixmap::fromImage(m_landmark_image));
    m_isDisplayingLandmarks = true;
}
