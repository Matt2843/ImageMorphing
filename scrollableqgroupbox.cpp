#include "scrollableqgroupbox.h"

#include <QHBoxLayout>

/**
 * @brief ScrollableQGroupBox::ScrollableQGroupBox
 * The default ScrollableQGroupBox constructor
 * @param parent
 * @param title
 */
ScrollableQGroupBox::ScrollableQGroupBox(QWidget *parent, const QString &title, Orientation Orientation) :
    QGroupBox(title, parent),
    m_content_pane(std::make_unique<QWidget>(nullptr)),
    m_scroll_area(std::make_unique<QScrollArea>(nullptr)),
    m_layout(std::make_unique<QVBoxLayout>(nullptr))
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
        m_content_pane_layout = std::make_unique<QVBoxLayout>(nullptr);
    else m_content_pane_layout = std::make_unique<QHBoxLayout>(nullptr);
    m_content_pane->setLayout(m_content_pane_layout.get());
    m_scroll_area->setWidget(m_content_pane.get());
    m_scroll_area->setWidgetResizable(true);
    m_layout->addWidget(m_scroll_area.get());
    setLayout(m_layout.get());
}

/**
 * @brief ScrollableQGroupBox::updatePreview
 * An auxillary protected class method to update the content_pane of this class
 * to be 1:1 withg the m_container (image database loaded).
 */
void ScrollableQGroupBox::updatePreview()
{
    for(const auto & data_point : m_container) {
        m_content_pane_layout->addWidget(data_point.get());
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
        m_content_pane_layout->removeWidget(data_point.get());
    }
    m_container.clear();
}
