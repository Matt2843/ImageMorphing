#include "labelledslidergroup.h"
#include <QDebug>

#include <QLabel>
#include <QSlider>
#include <QToolTip>
#include <QHBoxLayout>
#include <QVBoxLayout>

/**
 * @brief LabelledSliderGroup::LabelledSliderGroup
 *
 * The LabelledSliderGroup ctor, constructs a group of QLabelled QSliders.
 * The group aligns the sliders accroding to the Qt::Orientation parameter, which
 * can either be Qt::Horizontal or Qt::Vertical.
 *
 * @param labels a list of labels
 * @param parent the
 * @param orientation
 */
LabelledSliderGroup::LabelledSliderGroup(const QStringList &labels,
                                         Qt::Orientation orientation,
                                         QWidget *parent) :
    QWidget(parent),
    m_orientation(orientation),
    m_layout(new QHBoxLayout),
    m_label_layout(new QVBoxLayout),
    m_slider_layout(new QVBoxLayout)
{
    setup(labels);
}

/**
 * @brief LabelledSliderGroup::setup
 *
 * A private convenience method for setting up the layout and internal widgets of this container.
 *
 * @param labels the list of labels the class was constructed with.
 */
void LabelledSliderGroup::setup(const QStringList &labels)
{
    for(int i = 0; i < labels.size(); ++i) {
        QLabel *l = new QLabel(labels[i], this);
        m_labels.push_back(l);
        m_label_layout->addWidget(l);

        QSlider *s = new QSlider(m_orientation, this);
        s->setToolTip(QString::number(0));
        s->setRange(0, 100);
        s->setEnabled(false);
        m_sliders.push_back(s);
        m_slider_layout->addWidget(s);

        connect(s, &QSlider::sliderMoved, [=](int val){
            setSliderToolTip(val, i);
        });
    }

    m_layout->addLayout(m_label_layout);
    m_layout->addLayout(m_slider_layout);

    setLayout(m_layout);
}

/**
 * @brief LabelledSliderGroup::setLabelText
 *
 * A public method to set the text of a specific slider label
 *
 * @param which the label to be changed
 * @param text the input text
 * @param flag the text alignment
 */
void LabelledSliderGroup::setLabelText(unsigned long which, const QString &text, Qt::AlignmentFlag flag)
{
    if(which >= m_labels.size()) return;
    m_labels[which]->setText(text);
    m_labels[which]->setAlignment(flag);
}

/**
 * @brief LabelledSliderGroup::toggleSliders
 *
 * A public method to toggle sliders on/off
 *
 * @param from the start index
 * @param to the end index
 * @param on the toggle value
 */
void LabelledSliderGroup::toggleSliders(unsigned long from, unsigned long to, bool on)
{
    for(unsigned int i = from; i <= to; ++i) {
        m_sliders[i]->setEnabled(on);
    }
}

/**
 * @brief LabelledSliderGroup::resetSliders
 *
 * A public method to set the value of a range of sliders to 0.
 *
 * @param from the start index
 * @param to the end index
 */
void LabelledSliderGroup::resetSliders(unsigned long from, unsigned long to)
{
    for(unsigned int i = from; i <= to; ++i) {
        m_sliders[i]->setValue(0);
    }
}

/**
 * @brief LabelledSliderGroup::setSliderToolTip
 *
 * A private SLOT invoked when the slider is moved, to update its tooltip accordingly
 *
 * @param val the value the slider changed to
 * @param which the slider in question
 */
void LabelledSliderGroup::setSliderToolTip(int val, unsigned long which)
{
    if(which >= m_sliders.size()) return;
    m_sliders[which]->setToolTip(QString::number(val));
    QToolTip::showText(QCursor::pos(), QString::number(val), m_sliders[which]);
}

/**
 * @brief LabelledSliderGroup::getSlider
 * @param which the slider to be returned
 * @return m_sliders[which]
 */
QSlider* LabelledSliderGroup::getSlider(unsigned long which)
{
    if(which >= m_sliders.size()) return nullptr;
    return m_sliders[which];
}

/**
 * @brief LabelledSliderGroup::getSliderValue
 *
 * A convenience method to get the floating value range [0;1] of a slider
 *
 * @param which the slider to be evaluated
 * @return the floating value of the slider
 */
float LabelledSliderGroup::getSliderValue(unsigned long which)
{
    if(which >= m_sliders.size()) return 0.0;
    return (float)m_sliders[which]->value() / 100;
}
