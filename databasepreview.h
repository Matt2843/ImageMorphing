#pragma once
#include "scrollableqgroupbox.h"

class DatabasePreview : public ScrollableQGroupBox
{
    Q_OBJECT
public:
    explicit DatabasePreview(QWidget * parent = nullptr);

signals:
    void imageDoubleClicked(ImageContainer *img);
    void referenceImageRequest(ImageContainer *img, int number);

public slots:
    bool loadDatabaseFromFiles();
    bool loadDatabaseFromDirectory();

private slots:
    void imageRightClickInvoked(ImageContainer *img, QMouseEvent *event);

private:
    bool loadDatabase(const QStringList &image_file_paths);
    void scanImageResolutions(const QStringList &image_file_paths);
    bool imageResolutionDialog();
};
