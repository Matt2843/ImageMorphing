#pragma once

#include "imageprocessor.h"

#include <QGroupBox>

class ImageContainer;

class EditorPane : public QGroupBox {
    Q_OBJECT
public:
    explicit EditorPane(QWidget *parent = nullptr,
                        ImageContainer *m_reference_one = nullptr,
                        ImageContainer *m_reference_two = nullptr,
                        ImageContainer *m_target = nullptr);
    ~EditorPane() = default;

private:
    void setup();
    void setupConnections();

public:
    bool detectLandmarks(ImageContainer *img);

public slots:
    void m_morph_target_b_pressed();

private:

    ImageContainer *m_reference_one;
    ImageContainer *m_reference_two;
    ImageContainer *m_target;

    ImageProcessor *m_image_processor;

};
