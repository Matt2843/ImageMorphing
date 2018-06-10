#pragma once
#include <QGroupBox>

#include "imagecontainer.h"

#include <unordered_set>

#include <QString>
#include <QLayout>
#include <QWidget>
#include <QStringList>
#include <QScrollArea>
#include <QVBoxLayout>

class ScrollableQGroupBox : public QGroupBox {
    Q_OBJECT

public:
    enum Orientation {Vertical, Horizontal};
    explicit ScrollableQGroupBox(QWidget * parent = nullptr, const QString & title = "", Orientation Orientation = Vertical);
    ~ScrollableQGroupBox() = default;

private:
    void setup(Orientation orientation);

protected:
    void updatePreview();
    void clearContainerAndPreview();

protected:
    QWidget *m_content_pane;
    QScrollArea *m_scroll_area;
    QVBoxLayout *m_layout;
    QLayout *m_content_pane_layout;

    std::unordered_set<ImageContainer*> m_container;
};
