#pragma once
#include <QWidget>

#include <vector>

#include <QString>
#include <QStringList>

class QLabel;
class QSlider;
class QHBoxLayout;
class QVBoxLayout;
class LabelledSliderGroup : public QWidget
{
    Q_OBJECT
public:
    LabelledSliderGroup(const QStringList &labels, Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);
    ~LabelledSliderGroup() = default;

private:
    void setup(const QStringList &labels);

public:
    void setLabelText(unsigned long which, const QString &text, Qt::AlignmentFlag flag = Qt::AlignLeft);
    void toggleSliders(unsigned long from, unsigned long to, bool on);
    void resetSliders(unsigned long from, unsigned long to);

private slots:
    void setSliderToolTip(int val, unsigned long which);

public:
    QSlider* getSlider(unsigned long which);
    float getSliderValue(unsigned long which);

private:
    Qt::Orientation m_orientation;

    QHBoxLayout *m_layout;

    QVBoxLayout *m_label_layout;
    QVBoxLayout *m_slider_layout;

    std::vector<QLabel*> m_labels;
    std::vector<QSlider*> m_sliders;
};
