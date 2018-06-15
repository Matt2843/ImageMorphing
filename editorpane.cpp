#include "editorpane.h"

#include "imagecontainer.h"
#include "labelledslidergroup.h"

#include <memory>

#include <QDebug>

/**
 * @brief EditorPane::EditorPane
 * The default EditorPane constructor
 * @param parent
 */
EditorPane::EditorPane(QWidget *parent,
                       ImageContainer *m_reference_one,
                       ImageContainer *m_reference_two,
                       ImageContainer *m_target) :
    QGroupBox("Editor Pane", parent),
    m_layout(new QHBoxLayout()),
    m_reference_one(m_reference_one),
    m_reference_two(m_reference_two),
    m_target(m_target),
    m_image_processor(new ImageProcessor(this))
{
    setup();
    setupConnections();
}

/**
 * @brief EditorPane::setup
 */
void EditorPane::setup()
{
    QStringList slider_group_one_labels;
    slider_group_one_labels << "alpha" << "Homogeneous Filter" << "Gaussian Filter" << "Median Filter" << "Bilateral Filter";
    m_slider_group_one = new LabelledSliderGroup(slider_group_one_labels, this);
    m_slider_group_one->getSlider(ALPHA)->setValue(50);
    m_slider_group_one->getSlider(BILATERAL)->setRange(0,50);
    m_layout->addWidget(m_slider_group_one);
    setLayout(m_layout);
}

/**
 * @brief EditorPane::setupConnections
 */
void EditorPane::setupConnections()
{
    connect(m_slider_group_one->getSlider(ALPHA), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothMorph(int)));

    connect(m_slider_group_one->getSlider(HOMOGENEOUS), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothHomogeneous(int)));

    connect(m_slider_group_one->getSlider(GAUSSIAN), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothGaussian(int)));

    connect(m_slider_group_one->getSlider(MEDIAN), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothMedian(int)));

    connect(m_slider_group_one->getSlider(BILATERAL), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothBilateral(int)));
}

/**
 * @brief EditorPane::detectLandmarks
 * @param img
 * @return
 */
bool EditorPane::detectLandmarks(ImageContainer *img)
{
    if(img->hasLandmarks()) return false;
    if(!img->hasImage()) return false;
    std::string img_path = img->getImagePath().toString().toStdString();
    img->setLandmarks(m_image_processor->getFacialFeatures(img_path));
    return true;
}

/**
 * @brief EditorPane::setMorphReady
 * @param ready
 */
void EditorPane::setMorphReady(bool ready)
{
    if(ready) {
        m_slider_group_one->getSlider(ALPHA)->setEnabled(true);
    }
}

void EditorPane::toggleFilters(bool on)
{
    m_slider_group_one->toggleSliders(HOMOGENEOUS, BILATERAL, on);
}

/**
 * @brief EditorPane::m_morph_target_b_pressed
 */
void EditorPane::m_morph_target_b_pressed()
{
    m_image_processor->morphImages(m_reference_one,
                                   m_reference_two,
                                   m_target,
                                   m_slider_group_one->getSliderValue(ALPHA));
    toggleFilters(true);
}

/**
 * @brief EditorPane::smoothMorph
 * @param alpha
 */
void EditorPane::smoothMorph(int alpha)
{
    if(!m_reference_one->hasImage()         ||
       !m_reference_one->hasLandmarks()     ||
       !m_reference_two->hasImage()         ||
       !m_reference_two->hasLandmarks()) return;
    m_image_processor->morphImages(m_reference_one,
                                   m_reference_two,
                                   m_target,
                                   (float)alpha / 100);
}

#include <opencv2/highgui.hpp>

void EditorPane::applyFilters(ImageProcessor::Filter which, int intensity) const
{
    if(!m_target->hasImage()) return;
    QImage img = m_target->getSource();
    // Apply all current filters to the copy.
    int homogeneous_val = m_slider_group_one->getSlider(HOMOGENEOUS)->value();
    int gaussian_val = m_slider_group_one->getSlider(GAUSSIAN)->value();
    int median_val = m_slider_group_one->getSlider(MEDIAN)->value();
    int bilateral_val = m_slider_group_one->getSlider(BILATERAL)->value();
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::HOMOGENEOUS,
                                   homogeneous_val);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::GAUSSIAN,
                                   gaussian_val);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::MEDIAN,
                                   median_val);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::BILATERAL,
                                   bilateral_val);
    switch(which) {
    case ImageProcessor::Filter::HOMOGENEOUS:
        m_image_processor->applyFilter(img,
                                       ImageProcessor::Filter::HOMOGENEOUS,
                                       intensity);
        break;
    case ImageProcessor::Filter::GAUSSIAN:
        m_image_processor->applyFilter(img,
                                       ImageProcessor::Filter::GAUSSIAN,
                                       intensity);
        break;
    case ImageProcessor::Filter::MEDIAN:
        m_image_processor->applyFilter(img,
                                       ImageProcessor::Filter::MEDIAN,
                                       intensity);
        break;
    case ImageProcessor::Filter::BILATERAL:
        m_image_processor->applyFilter(img,
                                       ImageProcessor::Filter::BILATERAL,
                                       intensity);
        break;
    }
    m_target->setImage(QImage(img));
}

void EditorPane::smoothHomogeneous(int intensity)
{
    applyFilters(ImageProcessor::Filter::HOMOGENEOUS, intensity);
}

void EditorPane::smoothGaussian(int intensity)
{
    applyFilters(ImageProcessor::Filter::GAUSSIAN, intensity);
}

void EditorPane::smoothMedian(int intensity)
{
    applyFilters(ImageProcessor::Filter::MEDIAN, intensity);
}

void EditorPane::smoothBilateral(int intensity)
{
    applyFilters(ImageProcessor::Filter::BILATERAL, intensity);
}


