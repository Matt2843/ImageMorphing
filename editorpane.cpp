#include "editorpane.h"

#include "imagecontainer.h"
#include "labelledslidergroup.h"
#include "console.h"

#include <QDebug>
#include <QStringList>
#include <QFileDialog>
#include <QFile>

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
    m_image_processor(new ImageProcessor(this)),
    m_radio_buttons_container(new QGroupBox("Transform", this)),
    m_radio_buttons(new QButtonGroup(this)),
    m_r_normal(new QRadioButton("Normal", this)),
    m_r_grayscale(new QRadioButton("Grayscale", this)),
    m_r_fourier(new QRadioButton("Fourier", this)),
    m_b_add_to_results(new QPushButton("Add to results", this)),
    m_b_save_as(new QPushButton("Save as", this)),
    m_b_create_database(new QPushButton("Create database", this)),
    m_grayscale(false)
{
    setup();
    setupConnections();
}

/**
 * @brief EditorPane::setup
 */
void EditorPane::setup()
{
    m_b_add_to_results->setEnabled(false);
    m_b_save_as->setEnabled(false);
    m_b_create_database->setEnabled(false);

    m_r_normal->setChecked(true);
    m_r_normal->setEnabled(false);
    m_r_grayscale->setEnabled(false);
    m_r_fourier->setEnabled(false);

    QStringList slider_group_one_labels;
    slider_group_one_labels << "alpha" << "Homogeneous Filter" << "Gaussian Filter" << "Median Filter" << "Bilateral Filter";
    m_slider_group_one = new LabelledSliderGroup(slider_group_one_labels, this);
    m_slider_group_one->getSlider(ALPHA)->setValue(50);
    m_slider_group_one->getSlider(BILATERAL)->setRange(0,50);
    m_layout->addWidget(m_slider_group_one);

    QVBoxLayout *col_two_layout = new QVBoxLayout();
    QStringList slider_group_two_labels;
    slider_group_two_labels << "Sharpness" << "Contrast" << "Brightness";
    m_slider_group_two = new LabelledSliderGroup(slider_group_two_labels, this);

    m_radio_buttons->addButton(m_r_normal);
    m_radio_buttons->addButton(m_r_grayscale);
    m_radio_buttons->addButton(m_r_fourier);

    QHBoxLayout *radio_buttons_layout = new QHBoxLayout();
    radio_buttons_layout->addWidget(m_r_normal, 1, Qt::AlignCenter);
    radio_buttons_layout->addWidget(m_r_grayscale, 1, Qt::AlignCenter);
    radio_buttons_layout->addWidget(m_r_fourier, 1, Qt::AlignCenter);
    m_radio_buttons_container->setLayout(radio_buttons_layout);
    col_two_layout->addWidget(m_radio_buttons_container);
    col_two_layout->addWidget(m_slider_group_two);

    QHBoxLayout *col_two_double_button_layout = new QHBoxLayout();
    col_two_double_button_layout->addWidget(m_b_add_to_results);
    col_two_double_button_layout->addWidget(m_b_save_as);
    col_two_layout->addLayout(col_two_double_button_layout);

    col_two_layout->addWidget(m_b_create_database);

    m_layout->addLayout(col_two_layout);
    m_layout->setSpacing(0);
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

    connect(m_slider_group_two->getSlider(0), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothSharpness(int)));

    connect(m_slider_group_two->getSlider(1), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothContrast(int)));

    connect(m_slider_group_two->getSlider(2), SIGNAL(sliderMoved(int)),
            this, SLOT(smoothBrightness(int)));



    connect(m_b_add_to_results, SIGNAL(released()),
            this, SLOT(m_b_add_to_results_pressed()));

    connect(m_b_save_as, SIGNAL(released()),
            this, SLOT(m_b_save_as_pressed()));

    connect(m_b_create_database, SIGNAL(released()),
            this, SLOT(m_b_create_database_pressed()));
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
    img->setLandmarks(m_image_processor->getFacialFeatures(img));
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
    m_slider_group_two->toggleSliders(0, 2, on);
    m_b_add_to_results->setEnabled(on);
    m_b_save_as->setEnabled(on);
    m_b_create_database->setEnabled(on);
}

void EditorPane::resetSliders()
{
    m_slider_group_one->resetSliders(HOMOGENEOUS, BILATERAL);
    m_slider_group_two->resetSliders(0, 2);
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
    resetSliders();
    m_image_processor->morphImages(m_reference_one,
                                   m_reference_two,
                                   m_target,
                                   (float)alpha / 100);
}

void EditorPane::applyFilters(QImage &img) const
{
    if(!m_target->hasImage()) return;
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::HOMOGENEOUS,
                                   m_slider_group_one->getSlider(HOMOGENEOUS)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::GAUSSIAN,
                                   m_slider_group_one->getSlider(GAUSSIAN)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::MEDIAN,
                                   m_slider_group_one->getSlider(MEDIAN)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::BILATERAL,
                                   m_slider_group_one->getSlider(BILATERAL)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::SHARPNESS,
                                   m_slider_group_two->getSlider(0)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::CONTRAST,
                                   m_slider_group_two->getSlider(1)->value());
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::BRIGHTNESS,
                                   m_slider_group_two->getSlider(2)->value());
}

void EditorPane::smoothHomogeneous(int intensity)
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::HOMOGENEOUS,
                                   intensity);
    m_target->setImage(img);
}

void EditorPane::smoothGaussian(int intensity)
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::GAUSSIAN,
                                   intensity);
    m_target->setImage(img);
}

void EditorPane::smoothMedian(int intensity)
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::MEDIAN,
                                   intensity);
    m_target->setImage(img);
}

void EditorPane::smoothBilateral(int intensity)
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::BILATERAL,
                                   intensity);
    m_target->setImage(img);
}

void EditorPane::smoothSharpness(int intensity)
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::SHARPNESS,
                                   intensity);
    m_target->setImage(img);
}

void EditorPane::smoothContrast(int intensity)
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::CONTRAST,
                                   intensity);
    m_target->setImage(img);
}

void EditorPane::smoothBrightness(int intensity)
{
    QImage img = m_target->getSource();
    applyFilters(img);
    m_image_processor->applyFilter(img,
                                   ImageProcessor::Filter::BRIGHTNESS,
                                   intensity);
    m_target->setImage(img);
}

void EditorPane::m_r_normal_selected()
{

}

void EditorPane::m_r_grayscale_selected()
{

}

void EditorPane::m_r_fourier_selected()
{

}

void EditorPane::m_b_add_to_results_pressed()
{
    // set source to the temp source
    m_target->setSourceToTempSource();
    m_target->updateId();
    emit addToResultsInvoked(m_target);
}

void EditorPane::m_b_save_as_pressed()
{
    m_target->setSourceToTempSource();
    auto filename = QFileDialog::getSaveFileName(this,
                                                 tr("Save Image"),
                                                 m_target->getImageTitle(),
                                                 tr(".jpg"));
    if(filename.isEmpty()) return;
    auto save_status = m_target->getSource().save(filename + ".jpg");
    if(save_status)
        Console::appendToConsole("Saved morph: " + filename + ".jpg");
    else Console::appendToConsole("Failed to save: " + filename + ".jpg");
}

void EditorPane::m_b_create_database_pressed()
{
    qDebug() << "create database invoked";
}


