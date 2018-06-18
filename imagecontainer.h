#pragma once

#include <QLabel>

#include <vector>

#include <QUrl>
#include <QImage>
#include <QPoint>
#include <QString>
#include <QUuid>
#include <QScrollArea>

class ImageContainer : public QLabel
{
    Q_OBJECT
public:
    explicit ImageContainer(QWidget * parent = nullptr);
    ImageContainer(const ImageContainer &other);
    ~ImageContainer() = default;

public:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *event);

signals:
    void doubleClickDetected(ImageContainer *);
    void mousePressDetected(ImageContainer *, QMouseEvent *event);

public:
    void update(ImageContainer *other);

    bool setImageSource(const QString &path);
    void setImageSource(const QImage &source);
    void setImage(const QImage &image);
    void setSourceToTempSource();
    QImage getSource();
    QImage getTempSource();
    QImage getGrayscaleSource();

    QUrl getImagePath();
    QString getImageTitle();
    void setImageTitle(const QString &title);

    bool hasImage();

    void setLandmarks(const std::vector<QPoint> & landmarks, bool extra_landmarks = true);
    std::vector<QPoint> getLandmarks();
    bool hasLandmarks();
    bool hasBadLandmarks();
    void toggleLandmarks();
    void isDisplayingGrayscale(bool);

    void reset();

    void updateId();
    QString getId();

public:
    void displayOriginal();
    void displayLandmarks();
    void displayGrayscale();

private:
    void generateLandmarkImage();

private:
    QUrl m_img_path;
    QString m_img_title;

    QImage m_source;
    QImage m_temp_source;
    QImage m_grayscale_source;

    bool m_contains_image;

    QImage m_landmark_image;
    std::vector<QPoint> m_landmarks;

    bool m_isDisplayingLandmarks = false;
    bool m_isDisplayingGrayscale = false;
    QUuid m_id;

};
