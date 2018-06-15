#pragma once

#include "imageprocessor.h"

#include <QGroupBox>

#include <memory>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QStringList>

class ImageContainer;
class LabelledSliderGroup;
class EditorPane : public QGroupBox {
    Q_OBJECT
public:
    explicit EditorPane(QWidget *parent = nullptr,
                        ImageContainer *m_reference_one = nullptr,
                        ImageContainer *m_reference_two = nullptr,
                        ImageContainer *m_target = nullptr);
    ~EditorPane() = default;

    enum SLIDERS {
        ALPHA, HOMOGENEOUS, GAUSSIAN, MEDIAN, BILATERAL
    };

private:
    void setup();
    void setupConnections();

public:
    bool detectLandmarks(ImageContainer *img);
    void setMorphReady(bool ready = true);
    void toggleFilters(bool on);

public slots:
    void m_morph_target_b_pressed();
    void applyFilters(ImageProcessor::Filter which, int intensity) const;

private slots:
    void smoothMorph(int alpha);
    void smoothHomogeneous(int intensity);
    void smoothGaussian(int intensity);
    void smoothMedian(int intensity);
    void smoothBilateral(int intensity);

private:
    QHBoxLayout *m_layout;

    ImageContainer *m_reference_one;
    ImageContainer *m_reference_two;
    ImageContainer *m_target;

    ImageProcessor *m_image_processor;

    LabelledSliderGroup *m_slider_group_one;
};
