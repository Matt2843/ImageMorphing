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

signals:
    void doubleClickDetected(ImageContainer *);
    void mousePressDetected(ImageContainer *, QMouseEvent *event);

public:
    void update(ImageContainer *other);
    void reset();

public:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *event);

    bool setImageSource(const QString &path);
    void setImageSource(const QImage &source);
    void setImage(const QImage &image);

    void setLandmarks(const std::vector<QPoint> & landmarks,
                      bool extra_landmarks = true);
    bool hasBadLandmarks();

private:
    void generateLandmarkImage();

public:
    void displayOriginal();
    void displayLandmarks();
    void displayGrayscale();
    void toggleLandmarks();

public:
    QImage getSource();
    QImage getTempSource();
    QImage getGrayscaleSource();
    QUrl getImagePath();
    QString getImageTitle();
    void setImageTitle(const QString &title);
    bool hasImage();
    std::vector<QPoint> getLandmarks();
    bool hasLandmarks();
    void isDisplayingGrayscale(bool);
    void updateId();
    QString getId();

private:
    QImage m_source;
    QImage m_temp_source;
    QImage m_grayscale_source;
    QImage m_landmark_image;

    QUrl m_img_path;
    QString m_img_title;

    bool m_contains_image;
    bool m_isDisplayingLandmarks = false;
    bool m_isDisplayingGrayscale = false;

    std::vector<QPoint> m_landmarks;

    QUuid m_id;
};
