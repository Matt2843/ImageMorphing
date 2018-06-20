#pragma once
#include <QWidget>

#include "imageprocessor.h"

#include <vector>
#include <QString>

class ImageContainer;
class CommandLineMorphing : public QWidget {
    Q_OBJECT
public:
    explicit CommandLineMorphing(const QString &input_dir,
                                 const QString &output_dir,
                                 const QString &json_path = "");
    ~CommandLineMorphing() = default;

private:
    bool apply_settings();
    bool load_images();
    void morph_images();
    void apply_filters(QImage &img);

private:
    QString m_input_directory;
    QString m_output_directory;
    QString m_json_path;

private:
    int m_image_width;
    int m_image_height;
    float m_alpha;
    int m_h_filter;
    int m_g_filter;
    int m_m_filter;
    int m_b_filter;
    int m_transform;
    int m_sharpness;
    int m_contrast;
    int m_brightness;
    bool m_allow_bad_morphs;
    int m_format;
    ImageProcessor m_image_processor;
    std::vector<ImageContainer*> m_database;
};
