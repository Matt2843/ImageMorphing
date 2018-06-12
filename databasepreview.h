#pragma once
#include <QGroupBox>

#include "scrollableqgroupbox.h"

#include <QMouseEvent>

class DatabasePreview : public ScrollableQGroupBox {
    Q_OBJECT

public:
    explicit DatabasePreview(QWidget * parent = nullptr);

signals:
    void imageDoubleClicked(ImageContainer *img);
    void referenceImageRequest(ImageContainer *img, int number);

public slots:
    bool loadDatabaseFromFiles();
    bool loadDatabaseFromDirectory();

private:
    bool loadDatabase(const QStringList &image_file_paths);

private slots:
    void imageRightClickInvoked(ImageContainer *img, QMouseEvent *event);

public:
    static int m_image_width;
    static int m_image_height;
};
