#pragma once
#include <QGroupBox>

class ImageContainer;
class ImageProcessor;
class LabelledSliderGroup;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QButtonGroup;
class QRadioButton;
class EditorPane : public QGroupBox
{
    Q_OBJECT
public:
    explicit EditorPane(ImageContainer *ref_one = nullptr,
                        ImageContainer *ref_two = nullptr,
                        ImageContainer *target = nullptr,
                        QWidget *parent = nullptr);
    ~EditorPane() = default;

    enum SLIDERS {
        ALPHA, HOMOGENEOUS, GAUSSIAN, MEDIAN, BILATERAL
    };

private:
    void setup();
    void setupConnections();

signals:
    void addToResultsInvoked(ImageContainer*);

public:
    bool detectLandmarks(ImageContainer *img);
    void setMorphReady(bool ready = true);
    void toggleFilters(bool on);
    void resetSliders();
    void resetAll();

public slots:
    void m_morph_target_b_pressed();
    void applyFilters(QImage &img) const;

private slots:
    void smoothMorph();
    void smoothFilters();

    void m_r_normal_selected();
    void m_r_grayscale_selected();
    void m_r_fourier_selected();
    void m_b_add_to_results_pressed();
    void m_b_save_as_pressed();

private:
    QHBoxLayout *m_layout;

    ImageContainer *m_reference_one;
    ImageContainer *m_reference_two;
    ImageContainer *m_target;

    ImageProcessor *m_image_processor;

    QVBoxLayout *m_col_two_layout;
    QHBoxLayout *m_radio_buttons_layout;
    QGroupBox *m_radio_buttons_container;
    QButtonGroup *m_radio_buttons;
    QRadioButton *m_r_normal;
    QRadioButton *m_r_grayscale;
    QRadioButton *m_r_fourier;

    QPushButton *m_b_add_to_results;
    QPushButton *m_b_save_as;

    LabelledSliderGroup *m_slider_group_one;
    LabelledSliderGroup *m_slider_group_two;
};
