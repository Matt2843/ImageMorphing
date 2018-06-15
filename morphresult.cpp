#include "morphresult.h"

/**
 * @brief MorphResult::MorphResult
 * The default MorphResult ctor, it
 * @param ref_one
 * @param ref_two
 * @param target
 * @param alpha
 * @param homogeneous_val
 * @param gaussian_val
 * @param median_val
 * @param bilateral_val
 * @param sharpness_val
 * @param contrast_val
 * @param brightness_val
 * @param grayscale
 * @param parent
 */
MorphResult::MorphResult(ImageContainer *ref_one,
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
                         QWidget *parent)
    :   QWidget(parent),
        m_ref_one(ref_one),
        m_ref_two(ref_two),
        m_target(target),
        m_alpha(alpha),
        m_homogeneous_val(homogeneous_val),
        m_gaussian_val(gaussian_val),
        m_median_val(median_val),
        m_bilateral_val(bilateral_val),
        m_sharpness_val(sharpness_val),
        m_contrast_val(contrast_val),
        m_brightness_val(brightness_val),
        m_grayscale(grayscale) {}

MorphResult::MorphResult(const MorphResult &other) :
    QWidget((QWidget*)other.parent()),
    m_ref_one(other.m_ref_one),
    m_ref_two(other.m_ref_two),
    m_target(other.m_target),
    m_alpha(other.m_alpha),
    m_homogeneous_val(other.m_homogeneous_val),
    m_gaussian_val(other.m_gaussian_val),
    m_median_val(other.m_median_val),
    m_bilateral_val(other.m_bilateral_val),
    m_sharpness_val(other.m_sharpness_val),
    m_contrast_val(other.m_contrast_val),
    m_brightness_val(other.m_brightness_val),
    m_grayscale(other.m_grayscale) {}

//MorphResult::MorphResult(MorphResult &&other) noexcept :
//    m_ref_one(std::move(other.m_ref_one)),
//    m_ref_two(std::move(other.m_ref_two)),
//    m_target(std::move(other.m_target)),
//    m_alpha(std::move(other.m_alpha)),
//    m_homogeneous_val(std::move(other.m_homogeneous_val)),
//    m_gaussian_val(std::move(other.m_gaussian_val)),
//    m_median_val(std::move(other.m_median_val)),
//    m_bilateral_val(std::move(other.m_bilateral_val)),
//    m_sharpness_val(std::move(other.m_sharpness_val)),
//    m_contrast_val(std::move(other.m_contrast_val)),
//    m_brightness_val(std::move(other.m_brightness_val)),
//    m_grayscale(std::move(other.m_grayscale)) {}

std::vector<int> MorphResult::getSliderBatch()
{
    std::vector<int> batch = {m_homogeneous_val,
                              m_gaussian_val,
                              m_median_val,
                              m_bilateral_val,
                              m_sharpness_val,
                              m_contrast_val,
                              m_brightness_val};
    return batch;
}

float MorphResult::getAlpha()
{
    return m_alpha;
}

ImageContainer* MorphResult::getImageContainer(unsigned long which)
{
    switch(which) {
    case REF_ONE:
        return m_ref_one;
    case REF_TWO:
        return m_ref_two;
    case TARGET:
        return m_target;
    }
    return nullptr;
}

bool MorphResult::isGrayScale()
{
    return m_grayscale;
}
