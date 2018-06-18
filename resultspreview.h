#pragma once
#include "scrollableqgroupbox.h"

class ImageContainer;
class ResultsPreview : public ScrollableQGroupBox
{
    Q_OBJECT

public:
    explicit ResultsPreview(QWidget *parent = nullptr);

signals:
    void resultsNotEmpty();
    void resultsEmpty();

public slots:
    void addMorphResult(ImageContainer*);
    void removeMorphResult(ImageContainer *img, QMouseEvent *event);
    void exportResults(const char *format);
};
