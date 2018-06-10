#pragma once
#include <QGroupBox>

#include "imagecontainer.h"

#include <memory>
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
    std::unique_ptr<QWidget> m_content_pane;
    std::unique_ptr<QScrollArea> m_scroll_area;
    std::unique_ptr<QVBoxLayout> m_layout;
    std::unique_ptr<QLayout> m_content_pane_layout;

    std::unordered_set<std::unique_ptr<ImageContainer>> m_container;
};
