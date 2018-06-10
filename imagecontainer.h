#pragma once

#include <QLabel>

#include <memory>

#include <QUrl>
#include <QImage>
#include <QString>

class ImageContainer : public QLabel {
    Q_OBJECT
public:
    explicit ImageContainer(QWidget * parent = nullptr);
    ~ImageContainer() = default;

signals:
    void sourceChanged(QImage *);

public slots:
    void updatePixmap(QImage * image);

public:
    bool setImageSource(const QString & path);
    QImage * getSource();
    QUrl getImagePath();
    QString getImageTitle();

private:
    QUrl m_img_path;
    QString m_img_title;
    std::unique_ptr<QImage> m_source;
};
