#pragma once

#include <QLabel>

#include <vector>

#include <QUrl>
#include <QImage>
#include <QPoint>
#include <QString>

class ImageContainer : public QLabel {
    Q_OBJECT
public:
    explicit ImageContainer(QWidget * parent = nullptr);
    ~ImageContainer() = default;

public:
    void mouseDoubleClickEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *event);

signals:
    void sourceChanged(QImage *);
    void doubleClickDetected(ImageContainer *);
    void mousePressDetected(ImageContainer *, QMouseEvent *event);

public slots:
    void updatePixmap(QImage * image);

public:
    void update(ImageContainer * other);
    bool setImageSource(const QString & path);
    QImage * getSource();
    QUrl getImagePath();
    QString getImageTitle();
    bool hasImage();
    void setLandmarks(const std::vector<QPoint> & landmarks);
    std::vector<QPoint> getLandmarks();
    bool hasLandmarks();
    void toggleLandmarks();

private:
    void generateLandmarkImage();
    void displayOriginal();
    void displayLandmarks();

private:
    QUrl m_img_path;
    QString m_img_title;

    QImage m_source;

    bool m_contains_image;

    QImage m_landmark_image;
    std::vector<QPoint> m_landmarks;

    bool m_isDisplayingLandmarks = false;

};
