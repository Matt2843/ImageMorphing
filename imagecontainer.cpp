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
    m_contains_image(false),
    m_id(QUuid::createUuid())
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setScaledContents(true);
    setFrameShape(QFrame::Box);
    setLineWidth(1);
}

/**
 * @brief ImageContainer::ImageContainer
 * The default ImageContainer copy ctor
 * @param other
 */
ImageContainer::ImageContainer(const ImageContainer &other) :
    QLabel((QLabel*)other.parent()),
    m_img_path(other.m_img_path),
    m_img_title(other.m_img_title),
    m_source(other.m_source),
    m_temp_source(other.m_temp_source),
    m_grayscale_source(other.m_grayscale_source),
    m_contains_image(other.m_contains_image),
    m_landmark_image(other.m_landmark_image),
    m_landmarks(other.m_landmarks),
    m_isDisplayingLandmarks(other.m_isDisplayingLandmarks),
    m_isDisplayingGrayscale(other.m_isDisplayingGrayscale),
    m_id(other.m_id) {}

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
    m_temp_source = other->m_temp_source;
    m_grayscale_source = other->m_grayscale_source;
    m_contains_image = other->m_contains_image;
    m_landmark_image = other->m_landmark_image;
    m_landmarks = other->m_landmarks;
    m_isDisplayingLandmarks = other->m_isDisplayingLandmarks;
    m_isDisplayingGrayscale = other->m_isDisplayingGrayscale;
    if(m_isDisplayingGrayscale)
        setImage(m_grayscale_source);
    else setImage(m_temp_source);
    setToolTip(m_img_title);
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
    m_source = m_source.scaled(DatabasePreview::m_image_width, DatabasePreview::m_image_height,
                               Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_temp_source = m_source;
    m_grayscale_source = m_source.convertToFormat(QImage::Format_Grayscale8);
    m_contains_image = true;
    m_img_path = path;
    m_img_title = m_img_path.toString();
    m_img_title.replace(QRegExp("(.jpg)|(.png)|(.jpeg)"),"");
    m_img_title.replace(QRegExp(".*/"),"");
    resize(size());
    setPixmap(QPixmap::fromImage(m_source));
    setToolTip(m_img_title);
    return true;
}

/**
 * @brief ImageContainer::setImageSource
 * @param source
 */
void ImageContainer::setImageSource(const QImage &source)
{
    m_source = source.copy();
    m_temp_source = m_source;
    m_grayscale_source = source.convertToFormat(QImage::Format_Grayscale8);
    m_contains_image = true;
    m_isDisplayingGrayscale = false;
    resize(size());
    setPixmap(QPixmap::fromImage(m_source));
    setToolTip(m_img_title);
}

/**
 * @brief ImageContainer::setImage
 * @param image
 */
void ImageContainer::setImage(const QImage &image)
{
    m_temp_source = image;
    m_grayscale_source = m_temp_source.convertToFormat(QImage::Format_Grayscale8);
    m_contains_image = true;
    resize(size());
    setPixmap(QPixmap::fromImage(m_temp_source));
}

/**
 * @brief ImageContainer::setSourceToTempSource
 */
void ImageContainer::setSourceToTempSource()
{
    if(!m_temp_source.isNull())
        m_source = m_temp_source;
}

/**
 * @brief ImageContainer::getSource
 * @return
 */
QImage ImageContainer::getSource()
{
    return m_source;
}

/**
 * @brief ImageContainer::getTempSource
 * @return
 */
QImage ImageContainer::getTempSource()
{
    return m_temp_source;
}

/**
 * @brief ImageContainer::getGrayscaleSource
 * @return
 */
QImage ImageContainer::getGrayscaleSource()
{
    return m_grayscale_source;
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
 * @brief ImageContainer::setImageTitle
 * @param title
 */
void ImageContainer::setImageTitle(const QString &title)
{
    m_img_title = title;
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
void ImageContainer::setLandmarks(const std::vector<QPoint> &landmarks, bool extra_landmarks)
{
    m_isDisplayingLandmarks = false;
    m_landmarks = landmarks;
    if(extra_landmarks) {
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
    }
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
 * @brief ImageContainer::hasBadLandmarks
 * @return
 */
bool ImageContainer::hasBadLandmarks()
{
    if(m_landmarks.empty()) return true; // no landmarks to iterate
    QRect test(0, 0, DatabasePreview::m_image_width, DatabasePreview::m_image_height);
    for(const auto &point : m_landmarks) {
        if(!test.contains(point)) return true;
    }
    return false;
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

void ImageContainer::isDisplayingGrayscale(bool b)
{
    m_isDisplayingGrayscale = b;
}

void ImageContainer::reset()
{
    m_img_path = 0;
    m_img_title = "";
    m_source.fill(Qt::white);
    m_temp_source.fill(Qt::white);
    m_grayscale_source.fill(Qt::white);
    m_contains_image = false;
    m_landmark_image.fill(Qt::transparent);
    m_isDisplayingLandmarks = false;
    m_isDisplayingGrayscale = false;
    m_id = 0;

    setPixmap(QPixmap::fromImage(m_source));
}

/**
 * @brief ImageContainer::updateId
 */
void ImageContainer::updateId()
{
    m_id = QUuid::createUuid();
}

/**
 * @brief ImageContainer::getId
 * @return
 */
QString ImageContainer::getId()
{
    return m_id.toString();
}

/**
 * @brief ImageContainer::generateLandmarkImage
 */
void ImageContainer::generateLandmarkImage()
{
    QImage res(m_source.size(), QImage::Format_ARGB32);
    QPainter painter(&res);
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);

    painter.setPen(pen);
    painter.drawImage(QRect(0, 0, res.width(), res.height()), m_source, QRect(0, 0, res.width(), res.height()));
    for(const QPoint & landmark : m_landmarks) {
        pen.setColor(Qt::blue);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawEllipse(landmark, 5, 5);
        pen.setColor(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.drawPoint(landmark);
    }
    m_landmark_image = res;
}

void ImageContainer::displayOriginal()
{
    if(m_source.isNull()) return;
    resize(size());
    setPixmap(QPixmap::fromImage(m_source));
    m_isDisplayingLandmarks = false;
    m_isDisplayingGrayscale = false;
}

void ImageContainer::displayLandmarks()
{
    if(m_landmark_image.isNull()) return;
    resize(size());
    setPixmap(QPixmap::fromImage(m_landmark_image));
    m_isDisplayingLandmarks = true;
}

void ImageContainer::displayGrayscale()
{
    if(m_grayscale_source.isNull()) return;
    resize(size());
    setPixmap(QPixmap::fromImage(m_grayscale_source));
    m_isDisplayingGrayscale = true;
}
