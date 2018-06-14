#include "editorpane.h"

#include "imagecontainer.h"
#include "labelledslidergroup.h"

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
    slider_group_one_labels << "alpha" << "Normal Filter" << "Gaussian Filter" << "Median Filter" << "Bilateral Filter";
    m_slider_group_one = new LabelledSliderGroup(slider_group_one_labels, this);
    m_slider_group_one->getSlider(ALPHA)->setValue(50);
    m_layout->addWidget(m_slider_group_one);
    setLayout(m_layout);
}

/**
 * @brief EditorPane::setupConnections
 */
void EditorPane::setupConnections()
{

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
 * @brief EditorPane::m_morph_target_b_pressed
 */
void EditorPane::m_morph_target_b_pressed()
{
    qDebug() << "Morph pressed handle the returned morphed image.";
    m_image_processor->morphImages(m_reference_one, m_reference_two, m_target, 0.5);
}


