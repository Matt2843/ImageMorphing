#include "imagecontainer.h"

#include "databasepreview.h"
#include "globals.h"

#include <QMouseEvent>
#include <QRegExp>

#include <QPainter>
#include <QPen>

/**
 * @brief ImageContainer::ImageContainer
 *
 * The ImageContainer ctor, constructs the controls for the ImageContainer
 * class and sets the internal Qt sizing policies of this widget. The ctor
 * also initially creates a random UUID used to uniquely identify the image
 * contained in this class. The UUID is regenerated when the image is changed
 * in any form or way.
 *
 * @param parent the Qt widgets parent of this widget
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
 *
 * The default ImageContainer copy ctor
 *
 * @param other
 */
ImageContainer::ImageContainer(const ImageContainer &other) :
    QLabel((QLabel*)other.parent()),
    m_source(other.m_source),
    m_temp_source(other.m_temp_source),
    m_grayscale_source(other.m_grayscale_source),
    m_landmark_image(other.m_landmark_image),
    m_img_path(other.m_img_path),
    m_img_title(other.m_img_title),
    m_contains_image(other.m_contains_image),
    m_isDisplayingLandmarks(other.m_isDisplayingLandmarks),
    m_isDisplayingGrayscale(other.m_isDisplayingGrayscale),
    m_landmarks(other.m_landmarks),
    m_id(other.m_id) {}

/**
 * @brief ImageContainer::update
 *
 * A copy ctor -like public initializer invoked with a pointer to another
 * ImageContainer, the procedure copies the values of the other ptr, to this.
 *
 * This is a convenience method used to create copy ImageContainers to be used
 * in other Qt containers.
 *
 * @param other
 */
void ImageContainer::update(ImageContainer *other)
{
    m_source = other->m_source;
    m_temp_source = other->m_temp_source;
    m_grayscale_source = other->m_grayscale_source;
    m_landmark_image = other->m_landmark_image;
    m_img_path = other->m_img_path;
    m_img_title = other->m_img_title;
    m_contains_image = other->m_contains_image;
    m_isDisplayingLandmarks = other->m_isDisplayingLandmarks;
    m_isDisplayingGrayscale = other->m_isDisplayingGrayscale;
    m_landmarks = other->m_landmarks;
    if(m_isDisplayingGrayscale)
        setImage(m_grayscale_source);
    else setImage(m_temp_source);
    setToolTip(m_img_title);
}

/**
 * @brief ImageContainer::reset
 *
 * A convenience method invoked when the application-user creates a new project.
 *
 */
void ImageContainer::reset()
{
    m_source = QImage();
    m_temp_source = QImage();
    m_grayscale_source = QImage();
    m_landmark_image = QImage();
    m_img_path = 0;
    m_img_title = "";
    m_contains_image = false;
    m_isDisplayingLandmarks = false;
    m_isDisplayingGrayscale = false;
    m_landmarks.clear();
    m_id = 0;
    setPixmap(QPixmap::fromImage(m_source));
}

/**
 * @brief ImageContainer::mouseDoubleClickEvent
 *
 * Overriden QLabel mouseDoubleCickEvent, it merely invokes a custom created SIGNAL
 * namely doubleClickDetected(ImageContainer*) to make other Qt widgets aware of a d
 * ouble-click mouse event registered. The doubleClickDetected(ImageContainer*) signal
 * is in this application linked to the ImageEditors attemptToSetReferenceByDoubleClick(ImageContainer*)
 * SLOT, hence when the user double clicks an ImageContainer in the DatabasePreview view,
 * an empty image reference will be set if it exists.
 *
 */
void ImageContainer::mouseDoubleClickEvent(QMouseEvent *)
{
    emit doubleClickDetected(this);
}

/**
 * @brief ImageContainer::mousePressEvent
 *
 * Overriden QLabel mousePressEvent, emitting the mousePressDetected(ImageContainer*,QMouseEvent*) SIGNAL,
 * if the mouse-click corresponds to a right-mouse-button click. The SIGNAL is used to invoke referenceImageRequest()
 * public DatabasePreview SLOT. Hence, if the application-user right-clicks an ImageContainer in the DatabasePreview view,
 * the application-user is given the choice to set this ImageContainer as either a) reference one or b) reference two.
 *
 * @param event used to detect if a right-mouse-button click was registered
 */
void ImageContainer::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
        emit mousePressDetected(this, event);
}

/**
 * @brief ImageContainer::setImageSource
 *
 * Given a valid image file path, this method constructs an ImageContainer.
 *
 * @param path a valid image file path
 * @return true if construction was successful
 */
bool ImageContainer::setImageSource(const QString &path)
{
    auto loaded = m_source.load(path);
    if(!loaded) return false;
    m_source = m_source.scaled(fmg::Globals::img_width, fmg::Globals::img_height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    m_temp_source = m_source;
    m_grayscale_source = m_source.convertToFormat(QImage::Format_Grayscale8);
    m_img_path = path;
    m_img_title = m_img_path.toString();
    m_img_title.replace(QRegExp("(.jpg)|(.png)|(.jpeg)"),"");
    m_img_title.replace(QRegExp(".*/"),"");
    m_contains_image = true;
    resize(size());
    setPixmap(QPixmap::fromImage(m_source));
    setToolTip(m_img_title);
    return true;
}

/**
 * @brief ImageContainer::setImageSource
 *
 * Given a notNull() QImage source, this method constructs an ImageContainer.
 *
 * @param source a notNull() QImage source
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
 *
 * A public class method to update the image contained in this ImageContainer. Futhermore
 * the method creates a grayscale version copy of the input image.
 *
 * @param image a QImage updating this view
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
 * @brief ImageContainer::setLandmarks
 *
 * Sets the detected image landmarks and adds 8 extra landmarks to the corners/midpoints of
 * the scaled source image if desired.
 *
 * @param landmarks the set of detected facial features.
 * @param extra_landmarks true if 8 additional landmarks is wanted.
 */
void ImageContainer::setLandmarks(const std::vector<QPoint> &landmarks, bool extra_landmarks)
{
    m_isDisplayingLandmarks = false;
    m_landmarks = landmarks;
    if(extra_landmarks) {
        m_landmarks.push_back(QPoint(0, 0)); // top-left
        m_landmarks.push_back(QPoint(m_source.width() - 1, 0)); // top-right
        m_landmarks.push_back(QPoint(0, m_source.height() - 1)); // bot-left
        m_landmarks.push_back(QPoint(m_source.width() - 1, m_source.height() - 1)); // bot-right

        m_landmarks.push_back(QPoint(0, m_source.height() / 2)); // mid-left
        m_landmarks.push_back(QPoint(m_source.width() / 2, 0)); // mid-top
        m_landmarks.push_back(QPoint(m_source.width() - 1, m_source.height() / 2)); // mid-right
        m_landmarks.push_back(QPoint(m_source.width() / 2, m_source.height() - 1)); // mid-bot
    }
    generateLandmarkImage();
}

/**
 * @brief ImageContainer::hasBadLandmarks
 *
 * A public method to determine whether the contained image contains artificial landmarks,
 * i.e. landmarks which are not within the boundaries of the image.
 *
 * @return
 */
bool ImageContainer::hasBadLandmarks()
{
    if(m_landmarks.empty()) return true; // no landmarks to iterate
    QRect test(0, 0, fmg::Globals::img_width, fmg::Globals::img_height);
    for(const auto &point : m_landmarks) {
        if(!test.contains(point)) return true;
    }
    return false;
}

/**
 * @brief ImageContainer::generateLandmarkImage
 *
 * A convenience private method to generate a landmark overlay for the source image.
 *
 */
void ImageContainer::generateLandmarkImage()
{
    QImage res(m_source.size(), QImage::Format_ARGB32);
    QPainter painter(&res);
    QPen pen;
    pen.setCapStyle(Qt::RoundCap);

    painter.setPen(pen);
    painter.drawImage(QRect(0, 0, res.width(), res.height()), m_source, QRect(0, 0, res.width(), res.height()));
    int i = 0;
    for(const QPoint & landmark : m_landmarks) {
        pen.setColor(Qt::blue);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawPoint(landmark);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.drawText(landmark, QString::number(++i));
    }
    m_landmark_image = res;
}

/**
 * @brief ImageContainer::displayOriginal
 *
 * A public method to display the scaled source image.
 *
 */
void ImageContainer::displayOriginal()
{
    if(m_source.isNull()) return;
    resize(size());
    setPixmap(QPixmap::fromImage(m_source));
    m_isDisplayingLandmarks = false;
    m_isDisplayingGrayscale = false;
}

/**
 * @brief ImageContainer::displayLandmarks
 *
 * A public method to display the detected landmarks.
 *
 */
void ImageContainer::displayLandmarks()
{
    if(m_landmark_image.isNull()) return;
    resize(size());
    m_temp_source = m_landmark_image;
    setPixmap(QPixmap::fromImage(m_landmark_image));
    m_isDisplayingLandmarks = true;
}

/**
 * @brief ImageContainer::displayGrayscale
 *
 * A public method to display the grayscale image variant.
 *
 */
void ImageContainer::displayGrayscale()
{
    if(m_grayscale_source.isNull()) return;
    resize(size());
    setPixmap(QPixmap::fromImage(m_grayscale_source));
    m_isDisplayingGrayscale = true;
}

/**
 * @brief ImageContainer::toggleLandmarks
 *
 * A public method to toggle between the landmark overlay and the original source.
 *
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
 * @brief ImageContainer::getSource
 * @return m_source
 */
QImage ImageContainer::getSource()
{
    return m_source;
}

/**
 * @brief ImageContainer::getTempSource
 * @return m_temp_source
 */
QImage ImageContainer::getTempSource()
{
    return m_temp_source;
}

/**
 * @brief ImageContainer::getGrayscaleSource
 * @return m_grayscale_source
 */
QImage ImageContainer::getGrayscaleSource()
{
    return m_grayscale_source;
}

/**
 * @brief ImageContainer::getImagePath
 * @return m_img_path
 */
QUrl ImageContainer::getImagePath()
{
   return m_img_path;
}

/**
 * @brief ImageContainer::getImageTitle
 * @return m_img_title
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
 * @return m_contains_image
 */
bool ImageContainer::hasImage()
{
   return m_contains_image;
}

/**
 * @brief ImageContainer::getLandmarks
 * @return m_landmarks
 */
std::vector<QPoint> ImageContainer::getLandmarks()
{
    return m_landmarks;
}

/**
 * @brief ImageContainer::hasLandmarks
 * @return !m_landmarks.empty()
 */
bool ImageContainer::hasLandmarks()
{
    return !m_landmarks.empty();
}

/**
 * @brief ImageContainer::isDisplayingGrayscale
 * @param b
 */
void ImageContainer::isDisplayingGrayscale(bool b)
{
    m_isDisplayingGrayscale = b;
}

/**
 * @brief ImageContainer::updateId
 *
 * A public method to update the UUID of this ImageContainer
 *
 */
void ImageContainer::updateId()
{
    m_id = QUuid::createUuid();
}

/**
 * @brief ImageContainer::getId
 * @return m_id.toString() (QString)
 */
QString ImageContainer::getId()
{
    return m_id.toString();
}
