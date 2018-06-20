#pragma once
#include <QDialog>

#include "imageprocessor.h"

#include <vector>
#include <QString>

class QLabel;
class ImageContainer;
class LabelledSliderGroup;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class QGroupBox;
class QLineEdit;
class QCheckBox;
class MorphDatabaseDialog : public QDialog
{
    Q_OBJECT
public:
    MorphDatabaseDialog(QWidget *parent = nullptr, ImageContainer *preview = nullptr);
    ~MorphDatabaseDialog() = default;

private:
    void setup();
    void setupConnections();

private:
    void createPreview();
    void applyFilters(QImage &img);

private slots:
    void m_browse_in_dir_pressed();
    void m_browse_out_dir_pressed();
    void m_r_normal_selected();
    void m_r_grayscale_selected();
    void m_b_create_database_pressed();

    void m_alpha_changed();
    void m_slider_changed();

private:
    QString m_input_directory;
    QString m_output_directory;

    float m_alpha_val;
    unsigned long m_homogeneous_val;
    unsigned long m_gaussian_val;
    unsigned long m_median_val;
    unsigned long m_bilateral_val;
    unsigned long m_sharpness_val;
    unsigned long m_contrast_val;
    unsigned long m_brightness_val;
    bool m_grayscale;
    bool m_remove_bad_morphs;
    bool m_landmarks_detected;
    bool m_jpeg_format;

private:
    enum Sliders {
        ALPHA, HOMOGENEOUS, GAUSSIAN,
        MEDIAN, BILATERAL, SHARPNESS,
        CONTRAST, BRIGHTNESS
    };

    QVBoxLayout *m_layout;
    ImageContainer *m_one;
    ImageContainer *m_preview;
    ImageContainer *m_two;
    QLabel *m_preview_label;

    QHBoxLayout *m_in_dir_layout;
    QHBoxLayout *m_out_dir_layout;
    QLabel *m_in_dir;
    QLabel *m_out_dir;
    QLineEdit *m_in_dir_text;
    QLineEdit *m_out_dir_text;
    QPushButton *m_browse_in_dir;
    QPushButton *m_browse_out_dir;

    QLabel *m_resolution_label;
    QHBoxLayout *m_resolution_layout;
    QLabel *m_width;
    QLabel *m_height;
    QLineEdit *m_width_edit;
    QLineEdit *m_height_edit;

    LabelledSliderGroup *m_sliders;

    QHBoxLayout *m_radio_buttons_layout;
    QGroupBox *m_radio_buttons_container;
    QButtonGroup *m_radio_buttons;
    QRadioButton *m_r_normal;
    QRadioButton *m_r_grayscale;

    QGroupBox *m_format_radio_buttons_container;
    QButtonGroup *m_format_radio_buttons;
    QRadioButton *m_r_jpeg;
    QRadioButton *m_r_png;

    QHBoxLayout *m_bad_morphs_layout;
    QLabel *m_l_remove_bad_morphs;
    QCheckBox *m_cb_remove_bad_morphs;

    QHBoxLayout *m_buttons_layout;
    QPushButton *m_b_create_database;
    QPushButton *m_b_cancel;

    ImageProcessor m_image_processor;

    std::vector<ImageContainer*> m_database;
};
