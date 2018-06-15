#include "scrollableqgroupbox.h"

#include <QHBoxLayout>
#include <QDebug>

/**
 * @brief ScrollableQGroupBox::ScrollableQGroupBox
 * The default ScrollableQGroupBox constructor
 * @param parent
 * @param title
 */
ScrollableQGroupBox::ScrollableQGroupBox(QWidget *parent, const QString &title, Orientation Orientation) :
    QGroupBox(title, parent),
    m_content_pane(new QWidget(this)),
    m_scroll_area(new QScrollArea(this)),
    m_layout(new QVBoxLayout())
{
    setup(Orientation);
}

/**
 * @brief ScrollableQGroupBox::setup
 * An auxillary private class method to set up the internal
 * layout of this scrollable qgroupbox.
 */
void ScrollableQGroupBox::setup(Orientation orientation)
{
    if(orientation == Orientation::Vertical)
        m_content_pane_layout = new QVBoxLayout();
    else m_content_pane_layout = new QHBoxLayout();
    m_content_pane->setLayout(m_content_pane_layout);
    m_scroll_area->setWidget(m_content_pane);
    m_scroll_area->setWidgetResizable(true);
    m_layout->addWidget(m_scroll_area);
    setLayout(m_layout);
}

/**
 * @brief ScrollableQGroupBox::updatePreview
 * An auxillary protected class method to update the content_pane of this class
 * to be 1:1 withg the m_container (image database loaded).
 */
void ScrollableQGroupBox::updatePreview()
{
    for(const auto & data_point : m_container) {
        data_point->resize(m_scroll_area->width(), m_scroll_area->height() / 3);
        m_content_pane_layout->addWidget(data_point);
    }
}

/**
 * @brief ScrollableQGroupBox::clearContainerAndPreview
 * An auxillary protected class method to clear the m_container and
 * m_layout of this class.
 */
void ScrollableQGroupBox::clearContainerAndPreview()
{
    for(const auto & data_point : m_container) {
        m_content_pane_layout->removeWidget(data_point);
        delete data_point;
    }
    m_container.clear();
}
