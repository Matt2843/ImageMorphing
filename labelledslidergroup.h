#pragma once
#include <QWidget>

#include <vector>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QSlider>
#include <QString>
#include <QStringList>

class LabelledSliderGroup : public QWidget {
    Q_OBJECT
public:
    LabelledSliderGroup(const QStringList &labels, QWidget *parent = nullptr, Qt::Orientation orientation = Qt::Horizontal);
    ~LabelledSliderGroup() = default;

private:
    void setup(const QStringList &labels);

public:
    void setLabelText(unsigned long which, const QString &text, Qt::AlignmentFlag flag = Qt::AlignLeft);
    QSlider* getSlider(unsigned long which);
    float getSliderValue(unsigned long which);

private slots:
    void setSliderToolTip(int val, unsigned long which);

private:
    Qt::Orientation m_orientation;

    QHBoxLayout *m_layout;

    QVBoxLayout *m_label_layout;
    QVBoxLayout *m_slider_layout;

    std::vector<QLabel*> m_labels;
    std::vector<QSlider*> m_sliders;
};
