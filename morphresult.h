#pragma once
#include <QWidget>

#include "imagecontainer.h"

#include <vector>

class ImageContainer;
class MorphResult : public QWidget {
    Q_OBJECT
public:
    explicit MorphResult(ImageContainer *ref_one,
                         ImageContainer *ref_two,
                         ImageContainer *target,
                         float alpha,
                         int homogeneous_val,
                         int gaussian_val,
                         int median_val,
                         int bilateral_val,
                         int sharpness_val,
                         int contrast_val,
                         int brightness_val,
                         bool grayscale,
                         QWidget * parent = nullptr);
    MorphResult(const MorphResult &other);
    MorphResult(MorphResult &&other) noexcept;

    enum ImageContainers {
        REF_ONE, REF_TWO, TARGET
    };

public:
    std::vector<int> getSliderBatch();
    float getAlpha();
    ImageContainer* getImageContainer(unsigned long which);
    bool isGrayScale();

private:
    ImageContainer *m_ref_one;
    ImageContainer *m_ref_two;
    ImageContainer *m_target;

    float m_alpha;
    int m_homogeneous_val;
    int m_gaussian_val;
    int m_median_val;
    int m_bilateral_val;
    int m_sharpness_val;
    int m_contrast_val;
    int m_brightness_val;
    bool m_grayscale;
};
