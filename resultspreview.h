#pragma once
#include "scrollableqgroupbox.h"

#include "morphresult.h"

class ResultsPreview : public ScrollableQGroupBox
{
    Q_OBJECT

public:
    explicit ResultsPreview(QWidget *parent = nullptr);

public slots:
    void addMorphResult(MorphResult result);

private:
    std::vector<MorphResult> m_results;
};
