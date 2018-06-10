#include "imageeditor.h"

/**
 * @brief ImageEditor::ImageEditor
 * The default ImageEditor constructor
 * @param parent
 */
ImageEditor::ImageEditor(QWidget *parent) :
    QGroupBox("Image Editor", parent),
    m_layout(new QVBoxLayout()),
    m_reference_one(new ImageContainer(this)),
    m_reference_two(new ImageContainer(this)),
    m_target(new ImageContainer(this)),
    m_containers_layout(new QHBoxLayout()),
    m_label_layout(new QHBoxLayout()),
    m_ref_one_label(new QLabel("Reference One:", this)),
    m_target_label(new QLabel("Target:", this)),
    m_ref_two_label(new QLabel("Reference Two:", this)),
    m_editor_pane(new EditorPane(this))
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
    setLayout(m_layout);

    m_label_layout->addWidget(m_ref_one_label);
    m_label_layout->addWidget(m_target_label);
    m_label_layout->addWidget(m_ref_two_label);

    m_layout->addLayout(m_label_layout, 1);

    m_containers_layout->addWidget(m_reference_one);
    m_containers_layout->addWidget(m_target);
    m_containers_layout->addWidget(m_reference_two);

    m_layout->addLayout(m_containers_layout, 14);
    m_layout->addWidget(m_editor_pane, 9);
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

/**
 * @brief ImageEditor::setReferenceTwo
 * A public method for setting the second preview pane.
 * @param ref_two
 */
void ImageEditor::setReferenceTwo(ImageContainer *ref_two)
{
    m_reference_two->setImageSource(ref_two->getImagePath().toString());
}

/**
 * @brief ImageEditor::attemptToSetReference
 * When double-click is invoked in the database preview,
 * attempt to transfer that image container to a preview
 * success if one reference has not yet been set.
 * @param img
 */
void ImageEditor::attemptToSetReferenceByDoubleClick(ImageContainer *img)
{
    if(!m_reference_one->hasImage()) {
        setReferenceOne(img);
    } else if(!m_reference_two->hasImage()) {
        setReferenceTwo(img);
    }
}

/**
 * @brief ImageEditor::attemptToSetReference
 * Received a set reference signal with an image and a reference number
 * will attempt to set the reference accordingly.
 * @param img
 * @param number
 */
void ImageEditor::attemptToSetReference(ImageContainer *img, int number)
{
    if(number == 1) {
        setReferenceOne(img);
    } else if (number == 2) {
        setReferenceTwo(img);
    }
}
