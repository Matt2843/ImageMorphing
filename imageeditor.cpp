#include "imageeditor.h"

/**
 * @brief ImageEditor::ImageEditor
 * The default ImageEditor constructor
 * @param parent
 */
ImageEditor::ImageEditor(QWidget *parent) :
    QGroupBox("Image Editor", parent),
    m_layout(std::make_unique<QVBoxLayout>(nullptr)),
    m_reference_one(std::make_unique<ImageContainer>(nullptr)),
    m_reference_two(std::make_unique<ImageContainer>(nullptr)),
    m_target(std::make_unique<ImageContainer>(nullptr)),
    m_containers_layout(std::make_unique<QHBoxLayout>(nullptr)),
    m_label_layout(std::make_unique<QHBoxLayout>(nullptr)),
    m_ref_one_label(std::make_unique<QLabel>("Reference One:")),
    m_target_label(std::make_unique<QLabel>("Target:")),
    m_ref_two_label(std::make_unique<QLabel>("Reference Two:")),
    m_editor_pane(std::make_unique<EditorPane>(nullptr))
{
    setup();
}

/**
 * @brief ImageEditor::setup
 * An auxillary private class method for laying out the
 * image manipulators.
 */
void ImageEditor::setup()
{
    setLayout(m_layout.get());

    m_label_layout->addWidget(m_ref_one_label.get());
    m_label_layout->addWidget(m_target_label.get());
    m_label_layout->addWidget(m_ref_two_label.get());

    m_layout->addLayout(m_label_layout.get(), 1);

    m_containers_layout->addWidget(m_reference_one.get());
    m_containers_layout->addWidget(m_target.get());
    m_containers_layout->addWidget(m_reference_two.get());

    m_layout->addLayout(m_containers_layout.get(), 14);
    m_layout->addWidget(m_editor_pane.get(), 9);
}

/**
 * @brief ImageEditor::setReferenceOne
 * A public method for setting the first preview pane.
 * @param ref_one
 */
void ImageEditor::setReferenceOne(ImageContainer *ref_one)
{
    m_reference_one->setImageSource(ref_one->getImagePath().toString());
}

void ImageEditor::setReferenceTwo(ImageContainer *ref_two)
{
    m_reference_two->setImageSource(ref_two->getImagePath().toString());
}
