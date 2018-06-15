#include "imageeditor.h"

#include <QDebug>
#include <QPainter>

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
    m_ref_one_buttons(new QHBoxLayout()),
    m_ref_two_buttons(new QHBoxLayout()),
    m_reference_one_group(new QVBoxLayout()),
    m_reference_two_group(new QVBoxLayout()),
    m_target_group(new QVBoxLayout()),
    m_ref_one_detect_facial_landmarks_b(new QPushButton("Detect Landmarks", this)),
    m_ref_one_toggle_facial_landmarks_b(new QPushButton("Display Landmarks", this)),
    m_ref_two_detect_facial_landmarks_b(new QPushButton("Detect Landmarks", this)),
    m_ref_two_toggle_facial_landmarks_b(new QPushButton("Display Landmarks", this)),
    m_morph_target_b(new QPushButton("Morph", this)),
    m_containers_layout(new QHBoxLayout()),
    m_ref_one_label(new QLabel("Reference One:", this)),
    m_target_label(new QLabel("Target:", this)),
    m_ref_two_label(new QLabel("Reference Two:", this)),
    m_editor_pane(new EditorPane(this, m_reference_one, m_reference_two, m_target))
{
    setup();
    setupConnections();
}

/**
 * @brief ImageEditor::setup
 * An auxillary private class method for laying out the
 * image manipulators.
 */
void ImageEditor::setup()
{
    setLayout(m_layout);

    m_ref_one_detect_facial_landmarks_b->setEnabled(false);
    m_ref_two_detect_facial_landmarks_b->setEnabled(false);
    m_ref_one_toggle_facial_landmarks_b->setEnabled(false);
    m_ref_two_toggle_facial_landmarks_b->setEnabled(false);

    m_ref_one_buttons->addWidget(m_ref_one_detect_facial_landmarks_b);
    m_ref_one_buttons->addWidget(m_ref_one_toggle_facial_landmarks_b);

    m_reference_one_group->addWidget(m_ref_one_label, 1);
    m_reference_one_group->addWidget(m_reference_one, 15);
    m_reference_one_group->addLayout(m_ref_one_buttons);

    m_morph_target_b->setEnabled(false);
    m_target_group->addWidget(m_target_label, 1);
    m_target_group->addWidget(m_target, 15);
    m_target_group->addWidget(m_morph_target_b);

    m_ref_two_buttons->addWidget(m_ref_two_detect_facial_landmarks_b);
    m_ref_two_buttons->addWidget(m_ref_two_toggle_facial_landmarks_b);

    m_reference_two_group->addWidget(m_ref_two_label, 1);
    m_reference_two_group->addWidget(m_reference_two, 15);
    m_reference_two_group->addLayout(m_ref_two_buttons);

    m_containers_layout->addLayout(m_reference_one_group);
    m_containers_layout->addLayout(m_target_group);
    m_containers_layout->addLayout(m_reference_two_group);

    m_layout->addLayout(m_containers_layout, 14);
    m_layout->addWidget(m_editor_pane, 9);
}

/**
 * @brief ImageEditor::setupConnections
 * An auxillary method to set up the internal widgets connections.
 */
void ImageEditor::setupConnections()
{
    connect(m_ref_one_detect_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(detectLandmarksRefOne()));

    connect(m_ref_two_detect_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(detectLandmarksRefTwo()));

    connect(m_morph_target_b, SIGNAL(released()),
            this, SLOT(morphPressed()));

    connect(m_ref_one_toggle_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(toggleLandmarksRefOne()));

    connect(m_ref_two_toggle_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(toggleLandmarksRefTwo()));
}

/**
 * @brief ImageEditor::morphPressed
 */
void ImageEditor::morphPressed()
{
    if(!m_reference_one->hasLandmarks()) {
        detectLandmarksRefOne();
    }
    if(!m_reference_two->hasLandmarks()) {
        detectLandmarksRefTwo();
    }
    m_editor_pane->m_morph_target_b_pressed();

}

/**
 * @brief ImageEditor::detectLandmarksRefOne
 */
void ImageEditor::detectLandmarksRefOne()
{
    // TODO: RETURN .. detect landmarks in reference one via editor_pane->detectLandmarks(m_reference_one)
    // also implement an overlay QImage in the imagecontainer class to contain the detected landmarks
    // to instantly toggle between showing / not showing the detected landmarks.
    if(m_editor_pane->detectLandmarks(m_reference_one)) {
        m_ref_one_detect_facial_landmarks_b->setEnabled(false);
        m_ref_one_toggle_facial_landmarks_b->setEnabled(true);
    } else {
        m_ref_one_toggle_facial_landmarks_b->setEnabled(false);
    }
}

/**
 * @brief ImageEditor::detectLandmarksRefTwo
 */
void ImageEditor::detectLandmarksRefTwo()
{
    if(m_editor_pane->detectLandmarks(m_reference_two)) {
        m_ref_two_detect_facial_landmarks_b->setEnabled(false);
        m_ref_two_toggle_facial_landmarks_b->setEnabled(true);
    } else {
        m_ref_two_toggle_facial_landmarks_b->setEnabled(false);
    }
}

/**
 * @brief ImageEditor::toggleLandmarksRefOne
 */
void ImageEditor::toggleLandmarksRefOne()
{
    m_reference_one->toggleLandmarks();
}

/**
 * @brief ImageEditor::toggleLandmarksRefTwo
 */
void ImageEditor::toggleLandmarksRefTwo()
{
    m_reference_two->toggleLandmarks();
}

/**
 * @brief ImageEditor::testMorphConditions
 */
void ImageEditor::testMorphConditions()
{
    if(!m_reference_one->hasImage() || !m_reference_two->hasImage()) return;
    m_morph_target_b->setEnabled(true);
    m_editor_pane->setMorphReady();
}

/**
 * @brief ImageEditor::setReferenceOne
 * A public method for setting the first preview pane.
 * @param ref_one
 */
void ImageEditor::setReferenceOne(ImageContainer *ref_one)
{
    m_editor_pane->toggleFilters(false);
    m_reference_one->update(ref_one);
    if(m_reference_one->hasLandmarks()) {
        m_ref_one_detect_facial_landmarks_b->setEnabled(false);
        m_ref_one_toggle_facial_landmarks_b->setEnabled(true);
    } else {
        m_ref_one_detect_facial_landmarks_b->setEnabled(true);
        m_ref_one_toggle_facial_landmarks_b->setEnabled(false);
    }
    testMorphConditions();
}

/**
 * @brief ImageEditor::setReferenceTwo
 * A public method for setting the second preview pane.
 * @param ref_two
 */
void ImageEditor::setReferenceTwo(ImageContainer *ref_two)
{
    m_editor_pane->toggleFilters(false);
    m_reference_two->update(ref_two);
    if(m_reference_two->hasLandmarks()) {
        m_ref_two_detect_facial_landmarks_b->setEnabled(false);
        m_ref_two_toggle_facial_landmarks_b->setEnabled(true);
    } else {
        m_ref_two_detect_facial_landmarks_b->setEnabled(true);
        m_ref_two_toggle_facial_landmarks_b->setEnabled(false);
    }
    testMorphConditions();
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
