#include "scrollableqgroupbox.h"
#include <QDebug>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

/**
 * @brief ScrollableQGroupBox::ScrollableQGroupBox
 *
 * The default ScrollableQGroupBox ctor
 *
 * @param parent
 * @param title
 */
ScrollableQGroupBox::ScrollableQGroupBox(QWidget *parent, const QString &title, Orientation Orientation) :
    QGroupBox(title, parent),
    m_content_pane(new QWidget(this)),
    m_scroll_area(new QScrollArea(this)),
    m_layout(new QVBoxLayout)
{
    setup(Orientation);
}

/**
 * @brief ScrollableQGroupBox::~ScrollableQGroupBox
 *
 * The ScrollableQGroupBox destructor
 *
 */
ScrollableQGroupBox::~ScrollableQGroupBox()
{
    for(const auto &data_point : m_container) {
        delete data_point;
    }
}

/**
 * @brief ScrollableQGroupBox::setup
 *
 * A private convenience method for setting up the layout and internal widgets of this container.
 *
 */
void ScrollableQGroupBox::setup(Orientation orientation)
{
    if(orientation == Orientation::Vertical)
        m_content_pane_layout = new QVBoxLayout;
    else m_content_pane_layout = new QHBoxLayout;
    m_content_pane->setLayout(m_content_pane_layout);
    m_scroll_area->setWidget(m_content_pane);
    m_scroll_area->setWidgetResizable(true);
    m_layout->addWidget(m_scroll_area);
    setLayout(m_layout);
}

/**
 * @brief ScrollableQGroupBox::updatePreview
 *
 * A protected class method to update the content_pane of this class
 * to be 1:1 with the m_container (image database loaded).
 *
 */
void ScrollableQGroupBox::updatePreview()
{
    for(const auto & data_point : m_container) {
        m_content_pane_layout->addWidget(data_point);
    }
}

/**
 * @brief ScrollableQGroupBox::clearContainerAndPreview
 *
 * A public SLOT method to clear the m_container and m_layout of this class.
 *
 */
void ScrollableQGroupBox::clearContainerAndPreview()
{
    for(const auto & data_point : m_container) {
        m_content_pane_layout->removeWidget(data_point);
        delete data_point;
    }
    m_container.clear();
}
