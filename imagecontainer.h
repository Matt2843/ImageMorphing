#pragma once

#include <QLabel>

#include <vector>

#include <QUrl>
#include <QImage>
#include <QPoint>
#include <QString>
#include <QUuid>
#include <QScrollArea>

class ImageContainer : public QScrollArea
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

    QUrl getImagePath();
    QString getImageTitle();
    void setImageTitle(const QString &title);

    bool hasImage();

    void setLandmarks(const std::vector<QPoint> & landmarks, bool extra_landmarks = true);
    std::vector<QPoint> getLandmarks();
    bool hasLandmarks();
    void toggleLandmarks();

    void updateId();
    QString getId();

private:
    void generateLandmarkImage();
    void displayOriginal();
    void displayLandmarks();

private:
    QLabel m_img_label;

    QUrl m_img_path;
    QString m_img_title;

    QImage m_source;
    QImage m_temp_source;

    bool m_contains_image;

    QImage m_landmark_image;
    std::vector<QPoint> m_landmarks;

    bool m_isDisplayingLandmarks = false;

    QUuid m_id;
};
