#include "editorpane.h"
#include <QDebug>

#include "imagecontainer.h"
#include "imageprocessor.h"
#include "labelledslidergroup.h"
#include "console.h"

#include <QStringList>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QSlider>
#include <QFile>

/**
 * @brief EditorPane::EditorPane
 *
 * The EditorPane ctor, constructs the controls for the ImageEditor
 * class such as alpha-blend slider, filtering sliders, post-processing
 * transformations GUI widgets.
 *
 * @param ref_one the reference one image container passed by ImageEditor
 * @param ref_two the reference two image container passed by ImageEditor
 * @param target the morph target image container passed by ImageEditor
 * @param parent the Qt widgets parent of this widget.
 */
EditorPane::EditorPane(ImageContainer *ref_one,
                       ImageContainer *ref_two,
                       ImageContainer *target,
                       QWidget *parent) :
    QGroupBox("Editor Pane", parent),
    m_layout(new QHBoxLayout),
    m_reference_one(ref_one),
    m_reference_two(ref_two),
    m_target(target),
    m_image_processor(new ImageProcessor(this)),
    m_col_two_layout(new QVBoxLayout),
    m_radio_buttons_layout(new QHBoxLayout),
    m_radio_buttons_container(new QGroupBox("Transform", this)),
    m_radio_buttons(new QButtonGroup(this)),
    m_r_normal(new QRadioButton("Normal", this)),
    m_r_grayscale(new QRadioButton("Grayscale", this)),
    m_r_fourier(new QRadioButton("Fourier", this)),
    m_b_add_to_results(new QPushButton("Add to results", this)),
    m_b_save_as(new QPushButton("Save as", this))
{
    setup();
    setupConnections();
}

/**
 * @brief EditorPane::setup
 *
 * A private convenience method for setting up the layout and internal widgets of this container.
 *
 */
void EditorPane::setup()
{
    m_b_add_to_results->setEnabled(false);
    m_b_save_as->setEnabled(false);

    m_r_normal->setChecked(true);
    m_r_normal->setEnabled(false);
    m_r_grayscale->setEnabled(false);
    m_r_fourier->setEnabled(false);

    QStringList slider_group_one_labels;
    slider_group_one_labels << "alpha" << "Homogeneous Filter" << "Gaussian Filter" << "Median Filter" << "Bilateral Filter";
    m_slider_group_one = new LabelledSliderGroup(slider_group_one_labels, Qt::Horizontal, this);
    m_slider_group_one->getSlider(ALPHA)->setValue(50);
    m_slider_group_one->getSlider(BILATERAL)->setRange(0,50);
    m_layout->addWidget(m_slider_group_one);

    QStringList slider_group_two_labels;
    slider_group_two_labels << "Sharpness" << "Contrast" << "Brightness";
    m_slider_group_two = new LabelledSliderGroup(slider_group_two_labels, Qt::Horizontal, this);

    m_radio_buttons->addButton(m_r_normal);
    m_radio_buttons->addButton(m_r_grayscale);
    m_radio_buttons->addButton(m_r_fourier);

    m_radio_buttons_layout->addWidget(m_r_normal, 1, Qt::AlignCenter);
    m_radio_buttons_layout->addWidget(m_r_grayscale, 1, Qt::AlignCenter);
    m_radio_buttons_layout->addWidget(m_r_fourier, 1, Qt::AlignCenter);
    m_radio_buttons_container->setLayout(m_radio_buttons_layout);
    m_col_two_layout->addWidget(m_radio_buttons_container);
    m_col_two_layout->addWidget(m_slider_group_two);

    m_col_two_layout->addWidget(m_b_add_to_results);
    m_col_two_layout->addWidget(m_b_save_as);

    m_layout->addLayout(m_col_two_layout);
    m_layout->setSpacing(0);
    setLayout(m_layout);
}

/**
 * @brief EditorPane::setupConnections
 *
 * A private convenience method for setting up the Qt SLOTS/SIGNAL connections.
 *
 */
void EditorPane::setupConnections()
{
    connect(m_slider_group_one->getSlider(ALPHA), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothMorph()));

    connect(m_slider_group_one->getSlider(HOMOGENEOUS), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothFilters()));

    connect(m_slider_group_one->getSlider(GAUSSIAN), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothFilters()));

    connect(m_slider_group_one->getSlider(MEDIAN), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothFilters()));

    connect(m_slider_group_one->getSlider(BILATERAL), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothFilters()));

    connect(m_slider_group_two->getSlider(0), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothFilters()));

    connect(m_slider_group_two->getSlider(1), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothFilters()));

    connect(m_slider_group_two->getSlider(2), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothFilters()));

    connect(m_r_normal, SIGNAL(toggled(bool)),
            this, SLOT(m_r_normal_selected()));

    connect(m_r_grayscale, SIGNAL(toggled(bool)),
            this, SLOT(m_r_grayscale_selected()));

    connect(m_r_fourier, SIGNAL(toggled(bool)),
            this, SLOT(m_r_fourier_selected()));

    connect(m_b_add_to_results, SIGNAL(released()),
            this, SLOT(m_b_add_to_results_pressed()));

    connect(m_b_save_as, SIGNAL(released()),
            this, SLOT(m_b_save_as_pressed()));
}

/**
 * @brief EditorPane::detectLandmarks
 *
 * A public method invoked by the ImageEditor class when the
 * application-user presses the "Detect Landmarks" button.
 *
 * The ImageProcessor::getFacialFeatures(ImageContainer *img) is
 * invoked to initiate the dlib shape_predictor routine.
 *
 * @param img a image to invoke the dlib shape_predictor routine on
 * @return true if landmark detection was successful false otherwise
 */
bool EditorPane::detectLandmarks(ImageContainer *img)
{
    if(img->hasLandmarks()) return false;
    if(!img->hasImage()) return false;
    img->setLandmarks(m_image_processor->getFacialFeatures(img));
    return true;
}

/**
 * @brief EditorPane::setMorphReady
 *
 * A public method invoked by the ImageEditor class, signalling that
 * reference one & reference two has been set.
 *
 * @param ready indicates if the morphing procedure is ready or not
 */
void EditorPane::setMorphReady(bool ready)
{
    if(ready) {
        m_slider_group_one->getSlider(ALPHA)->setEnabled(true);
    }
}

/**
 * @brief EditorPane::toggleFilters
 *
 * A public method invoked by the ImageEditor class, usually invoked
 * when a morphed image has been produced to allow for application-user
 * post-processing. Alternatively if the application-user decides to
 * change the reference images, the post-processing sliders are turned off.
 *
 * @param on indicates if the filters should be turned on/off
 */
void EditorPane::toggleFilters(bool on)
{
    m_slider_group_one->toggleSliders(HOMOGENEOUS, BILATERAL, on);
    m_slider_group_two->toggleSliders(0, 2, on);
    m_b_add_to_results->setEnabled(on);
    m_b_save_as->setEnabled(on);
    m_r_normal->setEnabled(on);
    m_r_grayscale->setEnabled(on);
    m_r_fourier->setEnabled(on);
}

/**
 * @brief EditorPane::resetSliders
 *
 * A public method invoked byt he ImageEditor class, usually invoked
 * when the morphed output has changed.
 *
 */
void EditorPane::resetSliders()
{
    m_slider_group_one->resetSliders(HOMOGENEOUS, BILATERAL);
    m_slider_group_two->resetSliders(0, 2);
}


/**
 * @brief EditorPane::resetAll
 *
 * A public method used to reset all the GUI post-processing elements to their
 * default values, convenient when the user chooses to start a new project.
 *
 */
void EditorPane::resetAll()
{
    m_slider_group_one->toggleSliders(ALPHA, BILATERAL, false);
    m_slider_group_two->toggleSliders(0, 2, false);
    resetSliders();
    m_r_normal->setChecked(true);
    m_r_normal->setEnabled(false);
    m_r_grayscale->setEnabled(false);
    m_r_fourier->setEnabled(false);
    m_b_add_to_results->setEnabled(false);
    m_b_save_as->setEnabled(false);
}

/**
 * @brief EditorPane::m_morph_target_b_pressed
 *
 * A public Qt SLOT activated when the "Morph" button has been pressed.
 * The method starts the morphImages routine described in the ImageProcessor class.
 *
 */
void EditorPane::m_morph_target_b_pressed()
{
    m_image_processor->morphImages(m_reference_one,
                                   m_reference_two,
                                   m_target,
                                   m_slider_group_one->getSliderValue(ALPHA));
    m_r_normal->setChecked(true);
    toggleFilters(true);
}

/**
 * @brief EditorPane::applyFilters
 *
 * A public Qt SLOT activated when a filter slider has been changed by the
 * application-user applying the corresponding effects to the morphed result.
 *
 * @param img
 */
void EditorPane::applyFilters(QImage &img) const
{
    if(!m_target->hasImage()) return;
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::BRIGHTNESS,
                                   m_slider_group_two->getSlider(2)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::CONTRAST,
                                   m_slider_group_two->getSlider(1)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::SHARPNESS,
                                   m_slider_group_two->getSlider(0)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::BILATERAL,
                                   m_slider_group_one->getSlider(BILATERAL)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::MEDIAN,
                                   m_slider_group_one->getSlider(MEDIAN)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::GAUSSIAN,
                                   m_slider_group_one->getSlider(GAUSSIAN)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::HOMOGENEOUS,
                                   m_slider_group_one->getSlider(HOMOGENEOUS)->value());
}

/**
 * @brief EditorPane::smoothMorph
 *
 * A private slot invoked when the application-user adjusts the alpha slider.
 * The method invokes the morphImages routine of the ImageProcessor class and applies
 * the filter-values indicated by the sliders.
 *
 */
void EditorPane::smoothMorph()
{
    if(!m_reference_one->hasImage()         ||
       !m_reference_one->hasLandmarks()     ||
       !m_reference_two->hasImage()         ||
       !m_reference_two->hasLandmarks()) return;
    m_image_processor->morphImages(m_reference_one,
                                   m_reference_two,
                                   m_target,
                                   m_slider_group_one->getSliderValue(ALPHA));
    smoothFilters();
}

/**
 * @brief EditorPane::smoothFilters
 *
 * A private slot invoked when the application-user adjusts after effects sliders.
 * The method invokes the applyFilters(const QImage &img) method of this class. Futhermore
 * the method checks whether the filters should be applied to the fourier, grayscale or normal
 * transformed morphed result.
 *
 */
void EditorPane::smoothFilters()
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_target->setImage(img);
    if(m_r_grayscale->isChecked()) {
        m_target->displayGrayscale();
    } else if(m_r_fourier->isChecked()) {
        m_target->isDisplayingGrayscale(false);
        m_image_processor->fourierTransform(img);
        m_target->setImage(img);
    }
}

/**
 * @brief EditorPane::m_r_normal_selected
 *
 * A private SLOT activated when the user selects the normal transformation
 * radio button. The morphed result will be represented to the user without any
 * post-processing transformations applied.
 *
 */
void EditorPane::m_r_normal_selected()
{
    if(m_r_normal->isChecked()) {
        m_target->isDisplayingGrayscale(false);
        smoothFilters();
    }
}

/**
 * @brief EditorPane::m_r_grayscale_selected
 *
 * A private SLOT activated when the user selects the grayscale transformation
 * radio button. The morphed result will be represented to the user in grayscale.
 *
 */
void EditorPane::m_r_grayscale_selected()
{
    if(m_r_grayscale->isChecked()) {
        smoothFilters();
        m_target->displayGrayscale();
    }
}

/**
 * @brief EditorPane::m_r_fourier_selected
 *
 * A private SLOT activated when the user selects the fourier transformation
 * radio button. The morphed result will be represented to the user in the frequency domain.
 *
 */
void EditorPane::m_r_fourier_selected()
{
    if(m_r_fourier->isChecked()) {
        m_target->isDisplayingGrayscale(false);
        smoothFilters();
        QImage img = m_target->getTempSource();
        m_image_processor->fourierTransform(img);
        m_target->setImage(img);
    }
}

/**
 * @brief EditorPane::m_b_add_to_results_pressed
 *
 * A private SLOT activated when the application-user presses the add to results button.
 * The SLOT merely emits the addToResultsInvoked() SIGNAL, signalling the results preview
 * to update accordingly.
 *
 */
void EditorPane::m_b_add_to_results_pressed()
{
    emit addToResultsInvoked(m_target);
}

/**
 * @brief EditorPane::m_b_save_as_pressed
 *
 * A private SLOT activated when the application-user presses the save as button.
 * A standardized procedure of saving the current morphed result initializes.
 *
 */
void EditorPane::m_b_save_as_pressed()
{
    auto filename = QFileDialog::getSaveFileName(this,
                                                 tr("Save Image"),
                                                 m_target->getImageTitle(),
                                                 tr(".jpg"));
    if(filename.isEmpty()) return;
    auto save_status = m_target->getTempSource().save(filename + ".jpg");
    if(save_status)
        Console::appendToConsole("Saved morph: " + filename + ".jpg");
    else Console::appendToConsole("Failed to save: " + filename + ".jpg");
}


