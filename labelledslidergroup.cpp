#include "labelledslidergroup.h"

#include <QDebug>
#include <QToolTip>

/**
 * @brief LabelledSliderGroup::LabelledSliderGroup
 * The default labelledslidergroup constructor
 * @param texts
 * @param parent
 * @param orientation
 */
LabelledSliderGroup::LabelledSliderGroup(const QStringList &labels,
                                         QWidget *parent,
                                         Qt::Orientation orientation) :
    QWidget(parent),
    m_orientation(orientation),
    m_layout(new QHBoxLayout()),
    m_label_layout(new QVBoxLayout),
    m_slider_layout(new QVBoxLayout)
{
    setup(labels);
}

/**
 * @brief LabelledSliderGroup::setup
 * @param texts
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
 * @param text
 * @param which
 */
void LabelledSliderGroup::setLabelText(unsigned long which, const QString &text, Qt::AlignmentFlag flag)
{
    if(which >= m_labels.size()) return;
    m_labels[which]->setText(text);
    m_labels[which]->setAlignment(flag);
}

/**
 * @brief LabelledSliderGroup::getSlider
 * @param which
 * @return
 */
QSlider* LabelledSliderGroup::getSlider(unsigned long which)
{
    if(which >= m_sliders.size()) return nullptr;
    return m_sliders[which];
}

/**
 * @brief LabelledSliderGroup::getSliderValue
 * @param which
 * @return
 */
float LabelledSliderGroup::getSliderValue(unsigned long which)
{
    if(which >= m_sliders.size()) return 0.0;
    return (float)m_sliders[which]->value() / 100;
}

/**
 * @brief LabelledSliderGroup::toggleSliders
 */
void LabelledSliderGroup::toggleSliders(unsigned long from, unsigned long to, bool on)
{
    for(from; from <= to; ++from) {
        m_sliders[from]->setEnabled(on);
        if(!on) m_sliders[from]->setValue(0);
    }
}

/**
 * @brief LabelledSliderGroup::setSliderToolTip
 * @param s
 * @param val
 */
void LabelledSliderGroup::setSliderToolTip(int val, unsigned long which)
{
    if(which >= m_sliders.size()) return;
    m_sliders[which]->setToolTip(QString::number(val));
    QToolTip::showText(QCursor::pos(), QString::number(val), m_sliders[which]);
}







