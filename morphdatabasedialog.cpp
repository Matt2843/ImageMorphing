#include "morphdatabasedialog.h"
#include <QDebug>

#include "databasepreview.h"
#include "globals.h"

#include "imagecontainer.h"
#include "labelledslidergroup.h"

#include <cstdlib>

#include <QProgressDialog>
#include <QApplication>
#include <QDirIterator>
#include <QButtonGroup>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QSlider>
#include <QThread>
#include <QLabel>
#include <QDir>

/**
 * @brief MorphDatabaseDialog::MorphDatabaseDialog
 *
 * The MorphDatabaseDialog ctor, constructing the GUI container for the alternative
 * application view. The database dialog guides the user through applying the implemented
 * morphing techniques to an entire database.
 *
 * @param parent
 * @param preview
 */
MorphDatabaseDialog::MorphDatabaseDialog(QWidget *parent, ImageContainer *preview) :
    QDialog(parent),
    m_grayscale(false),
    m_remove_bad_morphs(true),
    m_landmarks_detected(false),
    m_jpeg_format(true),
    m_layout(new QVBoxLayout(this)),
    m_one(new ImageContainer),
    m_two(new ImageContainer),
    m_preview_label(new QLabel("<u>Preview</u>", this)),
    m_in_dir_layout(new QHBoxLayout),
    m_out_dir_layout(new QHBoxLayout),
    m_in_dir(new QLabel("<u>Input Directory</u>", this)),
    m_out_dir(new QLabel("<u>Output Directory</u>", this)),
    m_in_dir_text(new QLineEdit(this)),
    m_out_dir_text(new QLineEdit(this)),
    m_browse_in_dir(new QPushButton("Browse..", this)),
    m_browse_out_dir(new QPushButton("Browse..", this)),
    m_resolution_label(new QLabel("<u>Resolution</u>", this)),
    m_resolution_layout(new QHBoxLayout),
    m_width(new QLabel("width:", this)),
    m_height(new QLabel("height:", this)),
    m_width_edit(new QLineEdit(this)),
    m_height_edit(new QLineEdit(this)),
    m_radio_buttons_layout(new QHBoxLayout),
    m_radio_buttons_container(new QGroupBox("Transformation", this)),
    m_radio_buttons(new QButtonGroup(this)),
    m_r_normal(new QRadioButton("Normal", this)),
    m_r_grayscale(new QRadioButton("Grayscale", this)),
    m_format_radio_buttons_container(new QGroupBox("Format", this)),
    m_format_radio_buttons(new QButtonGroup(this)),
    m_r_jpeg(new QRadioButton("jpeg", this)),
    m_r_png(new QRadioButton("png", this)),
    m_bad_morphs_layout(new QHBoxLayout),
    m_l_remove_bad_morphs(new QLabel("Remove Bad Morphs", this)),
    m_cb_remove_bad_morphs(new QCheckBox(this)),
    m_buttons_layout(new QHBoxLayout),
    m_b_create_database(new QPushButton("Create Database", this)),
    m_b_cancel(new QPushButton("Cancel", this))
{
    if(preview != nullptr)
        m_preview->update(preview);
    setup();
    setupConnections();
}

/**
 * @brief MorphDatabaseDialog::~MorphDatabaseDialog
 *
 * The MorphDatabaseDialog destructor
 *
 */
MorphDatabaseDialog::~MorphDatabaseDialog()
{
    for(ImageContainer *img : m_database) delete img;
    m_database.clear();
}

/**
 * @brief MorphDatabaseDialog::setup
 *
 * A private convenience method for setting up the layout and internal widgets of this container.
 *
 */
void MorphDatabaseDialog::setup()
{
    m_preview = new ImageContainer(this);
    m_preview->setFixedSize(width() / 3, height() / 2);
    m_preview->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    m_layout->addWidget(m_preview_label, 0, Qt::AlignCenter);
    m_layout->addWidget(m_preview, 0, Qt::AlignCenter);

    m_layout->addWidget(m_in_dir);
    m_in_dir_text->setReadOnly(true);
    m_in_dir_layout->addWidget(m_in_dir_text, 3);
    m_in_dir_layout->addWidget(m_browse_in_dir, 1);
    m_layout->addLayout(m_in_dir_layout);

    m_layout->addWidget(m_out_dir);
    m_out_dir_text->setReadOnly(true);
    m_out_dir_layout->addWidget(m_out_dir_text, 3);
    m_out_dir_layout->addWidget(m_browse_out_dir, 1);
    m_layout->addLayout(m_out_dir_layout);

    m_width_edit->setReadOnly(true);
    m_height_edit->setReadOnly(true);
    m_layout->addWidget(m_resolution_label);
    m_resolution_layout->addWidget(m_width);
    m_resolution_layout->addWidget(m_width_edit);
    m_resolution_layout->addWidget(m_height);
    m_resolution_layout->addWidget(m_height_edit);
    m_layout->addLayout(m_resolution_layout);

    QStringList slider_names;
    slider_names << "alpha" << "Homogeneous Filter" <<
                    "Gaussian Filter" << "Median Filter" <<
                    "Bilateral Filter" << "Sharpness" <<
                    "Contrast" << "Brightness";
    m_sliders = new LabelledSliderGroup(slider_names, Qt::Horizontal, this);
    m_sliders->getSlider(ALPHA)->setValue(50);
    m_layout->addWidget(m_sliders);

    m_r_normal->setChecked(true);
    m_radio_buttons->addButton(m_r_normal);
    m_radio_buttons->addButton(m_r_grayscale);
    QHBoxLayout *radio_buttons_layout = new QHBoxLayout(m_radio_buttons_container);
    radio_buttons_layout->addWidget(m_r_normal, 1, Qt::AlignCenter);
    radio_buttons_layout->addWidget(m_r_grayscale, 1, Qt::AlignCenter);
    m_radio_buttons_layout->addWidget(m_radio_buttons_container);

    m_r_jpeg->setChecked(true);
    m_format_radio_buttons->addButton(m_r_jpeg);
    m_format_radio_buttons->addButton(m_r_png);
    QHBoxLayout *format_radio_buttons_layout = new QHBoxLayout(m_format_radio_buttons_container);
    format_radio_buttons_layout->addWidget(m_r_jpeg, 1, Qt::AlignCenter);
    format_radio_buttons_layout->addWidget(m_r_png, 1, Qt::AlignCenter);
    m_radio_buttons_layout->addWidget(m_format_radio_buttons_container);

    m_layout->addLayout(m_radio_buttons_layout);

    m_cb_remove_bad_morphs->setChecked(true);
    m_bad_morphs_layout->setAlignment(Qt::AlignCenter);
    m_bad_morphs_layout->addWidget(m_l_remove_bad_morphs);
    m_bad_morphs_layout->addWidget(m_cb_remove_bad_morphs);
    m_layout->addLayout(m_bad_morphs_layout);

    m_b_create_database->setEnabled(false);
    m_buttons_layout->addWidget(m_b_create_database);
    m_buttons_layout->addWidget(m_b_cancel);
    m_layout->addLayout(m_buttons_layout);

    setLayout(m_layout);
}

/**
 * @brief MorphDatabaseDialog::setupConnections
 *
 * A private convenience method for setting up the Qt SLOTS/SIGNAL connections.
 *
 */
void MorphDatabaseDialog::setupConnections()
{
    connect(m_browse_in_dir, SIGNAL(released()),
            this, SLOT(m_browse_in_dir_pressed()));

    connect(m_browse_out_dir, SIGNAL(released()),
            this, SLOT(m_browse_out_dir_pressed()));

    connect(m_r_normal, SIGNAL(toggled(bool)),
            this, SLOT(m_r_normal_selected()));

    connect(m_r_grayscale, SIGNAL(toggled(bool)),
            this, SLOT(m_r_grayscale_selected()));

    connect(m_r_jpeg, &QRadioButton::toggled, [&](){m_jpeg_format = !m_jpeg_format;});

    connect(m_b_create_database, SIGNAL(released()),
            this, SLOT(m_b_create_database_pressed()));

    connect(m_b_cancel, SIGNAL(released()),
            this, SLOT(deleteLater()));

    connect(m_b_cancel, SIGNAL(released()),
            this, SLOT(close()));

    connect(m_sliders->getSlider(ALPHA), SIGNAL(sliderMoved(int)),
            this, SLOT(m_alpha_changed()));

    connect(m_sliders->getSlider(HOMOGENEOUS), SIGNAL(sliderMoved(int)),
            this, SLOT(m_slider_changed()));

    connect(m_sliders->getSlider(GAUSSIAN), SIGNAL(sliderMoved(int)),
            this, SLOT(m_slider_changed()));

    connect(m_sliders->getSlider(MEDIAN), SIGNAL(sliderMoved(int)),
            this, SLOT(m_slider_changed()));

    connect(m_sliders->getSlider(BILATERAL), SIGNAL(sliderMoved(int)),
            this, SLOT(m_slider_changed()));

    connect(m_sliders->getSlider(SHARPNESS), SIGNAL(sliderMoved(int)),
            this, SLOT(m_slider_changed()));

    connect(m_sliders->getSlider(CONTRAST), SIGNAL(sliderMoved(int)),
            this, SLOT(m_slider_changed()));

    connect(m_sliders->getSlider(BRIGHTNESS), SIGNAL(sliderMoved(int)),
            this, SLOT(m_slider_changed()));

    connect(m_cb_remove_bad_morphs, &QCheckBox::toggled,
            [&](){m_remove_bad_morphs = !m_remove_bad_morphs;});
}

/**
 * @brief MorphDatabaseDialog::createPreview
 *
 * A private class method to create a convenient ImageContainer preview of how
 * the morphed results will look like after the application of the chosen after
 * effects.
 *
 */
void MorphDatabaseDialog::createPreview()
{
    QProgressDialog diag("Searching for a proper preview", "Abort", 0, m_database.size(), this);
    diag.setWindowModality(Qt::WindowModal);
    bool found_one = false;
    bool found_two = false;
    int count = 0;
    while(!found_one  || !found_two) {
        QApplication::processEvents();
        diag.setValue(diag.value() + 1);
        m_one = m_database[rand() % m_database.size()];
        m_two = m_database[rand() % m_database.size()];
        if(!found_one)
            m_one->setLandmarks(m_image_processor.getFacialFeatures(m_one));
        if(!m_one->hasBadLandmarks()) found_one = true;
        if(!found_two)
            m_two->setLandmarks(m_image_processor.getFacialFeatures(m_two));
        if(!m_two->hasBadLandmarks()) found_two = true;
        if(++count == 10) break;
    }
    diag.setValue(m_database.size());
    if(count != 10)
        m_image_processor.morphImages(m_one, m_two, m_preview, 0.5);
}

/**
 * @brief MorphDatabaseDialog::applyFilters
 *
 * A pricate convenience method to apply the specified filter values to a QImage reference.
 *
 * @param img the image the filters are applied to
 */
void MorphDatabaseDialog::applyFilters(QImage &img)
{
    if(img.isNull()) return;
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::BRIGHTNESS,
                                  m_sliders->getSlider(BRIGHTNESS)->value());
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::CONTRAST,
                                  m_sliders->getSlider(CONTRAST)->value());
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::SHARPNESS,
                                  m_sliders->getSlider(SHARPNESS)->value());
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::BILATERAL,
                                  m_sliders->getSlider(BILATERAL)->value());
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::MEDIAN,
                                  m_sliders->getSlider(MEDIAN)->value());
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::GAUSSIAN,
                                  m_sliders->getSlider(GAUSSIAN)->value());
    m_image_processor.applyFilter(img,
                                  ImageProcessor::Filter::HOMOGENEOUS,
                                  m_sliders->getSlider(HOMOGENEOUS)->value());
}

/**
 * @brief MorphDatabaseDialog::m_browse_in_dir_pressed
 *
 * A private SLOT invoked when the user invokes the browse input directory action.
 *
 * This SLOT invokes a load-directory routine, which loads copies of scaled image sources
 * into the application used as targets for the morphing routine.
 *
 */
void MorphDatabaseDialog::m_browse_in_dir_pressed()
{
    QString directory_path = QFileDialog::getExistingDirectory(this,
                                                               tr("Select Input Directory"),
                                                               QDir::currentPath(),
                                                               QFileDialog::ShowDirsOnly |
                                                               QFileDialog::DontResolveSymlinks);
    if(directory_path.isEmpty()) return;
    if(!m_database.empty()) {
        for(ImageContainer *img : m_database) {
            delete img;
        }
        m_database.clear();
    }
    m_input_directory = directory_path;
    m_in_dir_text->setText(m_input_directory);
    QDir directory(directory_path);
    directory.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    directory.setNameFilters(QStringList() << "*.jpg" << "*.png" << "*.jpeg");
    QDirIterator it(directory);

    QStringList image_paths;

    std::vector<int> widths;
    std::vector<int> heights;

    while(it.hasNext()) {
        image_paths << it.next();
        QImage img(it.filePath());
        widths.push_back(img.width());
        heights.push_back(img.height());
    }

    fmg::Globals::img_width = (int)(*std::min_element(widths.begin(), widths.end()));
    fmg::Globals::img_height = (int)(*std::min_element(heights.begin(), heights.end()));

    if(image_paths.size() <= 2) return;

    m_width_edit->setText(QString::number(fmg::Globals::img_width));
    m_height_edit->setText(QString::number(fmg::Globals::img_height));

    QProgressDialog diag("Loading files...", "Abort", 0, directory.count(), this);
    diag.setWindowModality(Qt::WindowModal);
    for(const auto &path : image_paths) {
        QApplication::processEvents();
        diag.setValue(diag.value() + 1);
        ImageContainer *img = new ImageContainer(this);
        img->setImageSource(path);
        m_database.push_back(img);
    }

    createPreview();

    if(m_preview->hasImage()) {
        m_sliders->toggleSliders(ALPHA, BRIGHTNESS, true);
    }
    if(!m_in_dir_text->text().isEmpty() && !m_out_dir_text->text().isEmpty()) m_b_create_database->setEnabled(true);
    m_landmarks_detected = false;
}

/**
 * @brief MorphDatabaseDialog::m_browse_out_dir_pressed
 *
 * A private SLOT invoked when the user invokes the browse output directory action.
 *
 */
void MorphDatabaseDialog::m_browse_out_dir_pressed()
{
    QString directory_path = QFileDialog::getExistingDirectory(this,
                                                               tr("Select Output Directory"),
                                                               QDir::currentPath(),
                                                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(directory_path.isEmpty()) return;
    m_output_directory = directory_path;
    m_out_dir_text->setText(m_output_directory);
    if(!m_in_dir_text->text().isEmpty() && !m_out_dir_text->text().isEmpty()) m_b_create_database->setEnabled(true);
}

/**
 * @brief MorphDatabaseDialog::m_r_normal_selected
 *
 * A private SLOT invoked when the normal transformation radio button is selected.
 *
 */
void MorphDatabaseDialog::m_r_normal_selected()
{
    if(m_r_normal->isChecked()) {
        QImage img = m_preview->getSource();
        applyFilters(img);
        m_preview->setImage(img);
        m_grayscale = false;
    }
}

/**
 * @brief MorphDatabaseDialog::m_r_grayscale_selected
 *
 * A private SLOT invoked when the grayscale transformation radio button is selected.
 *
 */
void MorphDatabaseDialog::m_r_grayscale_selected()
{
    if(m_r_grayscale->isChecked()) {
        m_preview->displayGrayscale();
        m_grayscale = true;
    }
}

/**
 * @brief MorphDatabaseDialog::m_b_create_database_pressed
 *
 * A private SLOT invoked when the create database button is pressed.
 *
 * This SLOT invokes the routine of morphing the images of the input-directory
 * and creates a database of morphed images with post-processed effects applied
 * according to the values selected by the application-user.
 *
 */
void MorphDatabaseDialog::m_b_create_database_pressed()
{
    if(!QDir(m_out_dir_text->text()).exists()) return; // the directory does not exist.

    if(!m_landmarks_detected) {
        QProgressDialog landmarks_diag("Detecting Landmarks...", "Abort", 0, m_database.size(), this);
        landmarks_diag.setWindowModality(Qt::WindowModal);
        landmarks_diag.setValue(landmarks_diag.value() + 1);
        for(ImageContainer *img : m_database) {
            if(landmarks_diag.wasCanceled()) break;
            landmarks_diag.setValue(landmarks_diag.value() + 1);
            if(img->hasLandmarks()) continue;
            img->setLandmarks(m_image_processor.getFacialFeatures(img));
        }
        if(!landmarks_diag.wasCanceled()) m_landmarks_detected = true;
    }

    QProgressDialog diag("Creating Morphs...", "Abort", 0, m_database.size(), this);
    diag.setWindowModality(Qt::WindowModal);
    for(ImageContainer *one : m_database) {
        QApplication::processEvents();
        diag.setValue(diag.value() + 1);
        if(one->hasBadLandmarks() && m_remove_bad_morphs) continue;
        for(auto it = m_database.begin(); it != m_database.end(); ++it) {
            if(diag.wasCanceled()) break;
            if(*it == one) continue;
            ImageContainer *two = *it;
            if(two->hasBadLandmarks() && m_remove_bad_morphs) continue;
            ImageContainer target;
            m_image_processor.morphImages(one, two, &target,
                                          m_sliders->getSliderValue(ALPHA));
            QImage img = target.getSource();
            applyFilters(img);
            target.setImage(img);
            QString format = m_jpeg_format ? ".jpg" : ".png";
            if(m_grayscale) {
                target.getGrayscaleSource().save(m_out_dir_text->text() + "/" + "g_" + target.getImageTitle() + target.getId() + format);
            } else {
                target.getTempSource().save(m_out_dir_text->text() + "/" + target.getImageTitle() + target.getId() + format);
            }
        }
    }
    diag.setValue(m_database.size());
}

/**
 * @brief MorphDatabaseDialog::m_alpha_changed
 *
 * A private SLOT invoked when the alpha slider is moved by the application-user.
 *
 */
void MorphDatabaseDialog::m_alpha_changed()
{
    m_image_processor.morphImages(m_one, m_two, m_preview,
                                  m_sliders->getSliderValue(ALPHA));
    m_slider_changed();
}

/**
 * @brief MorphDatabaseDialog::m_slider_changed
 *
 * A private SLOT invoked when a filter slider is moved by the application-user.
 *
 */
void MorphDatabaseDialog::m_slider_changed()
{
    QImage img = m_preview->getSource();
    applyFilters(img);
    m_preview->setImage(img);
    if(m_grayscale) m_preview->displayGrayscale();
}
