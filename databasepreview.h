#pragma once
#include <QGroupBox>

#include "scrollableqgroupbox.h"


class DatabasePreview : public ScrollableQGroupBox {
    Q_OBJECT

public:
    explicit DatabasePreview();

public slots:
    bool loadDatabaseFromFiles();
    bool loadDatabaseFromDirectory();

private:
    bool loadDatabase(const QStringList & image_file_paths);
};
