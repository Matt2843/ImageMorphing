#include "imageeditor.h"
#include <QDebug>

#include "editorpane.h"
#include "imagecontainer.h"
#include "console.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>

/**
 * @brief ImageEditor::ImageEditor
 *
 * The default ImageEditor ctor, constructs the controls for the ImageEditor class.
 *
 * @param parent
 */
ImageEditor::ImageEditor(QWidget *parent) :
    QGroupBox("Image Editor", parent),
    m_layout(new QVBoxLayout),
    m_reference_one(new ImageContainer(this)),
    m_reference_two(new ImageContainer(this)),
    m_target(new ImageContainer(this)),
    m_ref_one_buttons(new QHBoxLayout),
    m_ref_two_buttons(new QHBoxLayout),
    m_target_buttons(new QHBoxLayout),
    m_reference_one_group(new QVBoxLayout),
    m_reference_two_group(new QVBoxLayout),
    m_target_group(new QVBoxLayout),
    m_ref_one_detect_facial_landmarks_b(new QPushButton("Detect Landmarks", this)),
    m_ref_one_toggle_facial_landmarks_b(new QPushButton("Toggle Landmarks", this)),
    m_ref_two_detect_facial_landmarks_b(new QPushButton("Detect Landmarks", this)),
    m_ref_two_toggle_facial_landmarks_b(new QPushButton("Toggle Landmarks", this)),
    m_morph_target_b(new QPushButton("Morph", this)),
    m_target_toggle_facial_landmarks_b(new QPushButton("Toggle Landmarks", this)),
    m_containers_layout(new QHBoxLayout),
    m_ref_one_label(new QLabel("Reference One:", this)),
    m_target_label(new QLabel("Target:", this)),
    m_ref_two_label(new QLabel("Reference Two:", this)),
    m_editor_pane(new EditorPane(m_reference_one, m_reference_two, m_target, this)),
    console(new Console(this))
{
    setup();
    setupConnections();
}

/**
 * @brief ImageEditor::setup
 *
 * A private convenience method for setting up the layout and internal widgets of this container.
 *
 */
void ImageEditor::setup()
{
    m_ref_one_detect_facial_landmarks_b->setEnabled(false);
    m_ref_two_detect_facial_landmarks_b->setEnabled(false);
    m_ref_one_toggle_facial_landmarks_b->setEnabled(false);
    m_ref_two_toggle_facial_landmarks_b->setEnabled(false);
    m_morph_target_b->setEnabled(false);
    m_target_toggle_facial_landmarks_b->setEnabled(false);

    m_ref_one_buttons->addWidget(m_ref_one_detect_facial_landmarks_b);
    m_ref_one_buttons->addWidget(m_ref_one_toggle_facial_landmarks_b);

    m_reference_one_group->addWidget(m_ref_one_label, 1);
    m_reference_one_group->addWidget(m_reference_one, 15);
    m_reference_one_group->addLayout(m_ref_one_buttons);

    m_target_buttons->addWidget(m_morph_target_b);
    m_target_buttons->addWidget(m_target_toggle_facial_landmarks_b);

    m_target_group->addWidget(m_target_label, 1);
    m_target_group->addWidget(m_target, 15);
    m_target_group->addLayout(m_target_buttons);

    m_ref_two_buttons->addWidget(m_ref_two_detect_facial_landmarks_b);
    m_ref_two_buttons->addWidget(m_ref_two_toggle_facial_landmarks_b);

    m_reference_two_group->addWidget(m_ref_two_label, 1);
    m_reference_two_group->addWidget(m_reference_two, 15);
    m_reference_two_group->addLayout(m_ref_two_buttons);

    m_containers_layout->addLayout(m_reference_one_group);
    m_containers_layout->addLayout(m_target_group);
    m_containers_layout->addLayout(m_reference_two_group);

    m_layout->addLayout(m_containers_layout, 6);
    m_layout->addWidget(m_editor_pane, 2);
    m_layout->addWidget(console, 1);
    setLayout(m_layout);
}

/**
 * @brief ImageEditor::setupConnections
 *
 * A private convenience method for setting up the Qt SLOTS/SIGNAL connections.
 *
 */
void ImageEditor::setupConnections()
{
    connect(m_ref_one_detect_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(detectLandmarksRefOne()));

    connect(m_ref_two_detect_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(detectLandmarksRefTwo()));

    connect(m_morph_target_b, SIGNAL(released()),
            this, SLOT(morphPressed()));

    connect(m_target_toggle_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(toggleLandmarksTarget()));

    connect(m_ref_one_toggle_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(toggleLandmarksRefOne()));

    connect(m_ref_two_toggle_facial_landmarks_b, SIGNAL(released()),
            this, SLOT(toggleLandmarksRefTwo()));
}

/**
 * @brief ImageEditor::setReferenceOne
 *
 * A public method for setting the first preview pane.
 *
 * @param ref_one a ptr to the ImageContainer selected from DatabasePreview
 */
void ImageEditor::setReferenceOne(ImageContainer *ref_one)
{
    m_editor_pane->toggleFilters(false);
    m_editor_pane->resetSliders();
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
 *
 * A public method for setting the second preview pane.
 *
 * @param ref_two a ptr to the ImageContainer selected from DatabasePreview
 */
void ImageEditor::setReferenceTwo(ImageContainer *ref_two)
{
    m_editor_pane->toggleFilters(false);
    m_editor_pane->resetSliders();
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
 *
 * When double-click is invoked in the database preview, attempt to transfer that
 * image container to a preview success if one reference has not yet been set.
 *
 * @param img a ptr to the ImageContainer selected from DatabasePreview
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
 *
 * A public SLOT invoked when an attemptToSetReference() signal is emitted.
 *
 * @param img a ptr to the ImageContainer selected from DatabasePreview
 * @param number either one or two depending on which reference to be set.
 */
void ImageEditor::attemptToSetReference(ImageContainer *img, int number)
{
    if(number == 1) {
        setReferenceOne(img);
    } else if (number == 2) {
        setReferenceTwo(img);
    }
}

/**
 * @brief ImageEditor::resetAll
 *
 * A public SLOT for resetting the editor pane and image editor invoked when
 * the application-user starts a new project.
 *
 */
void ImageEditor::resetAll()
{
    m_editor_pane->resetAll();

    m_ref_one_detect_facial_landmarks_b->setEnabled(true);
    m_ref_two_detect_facial_landmarks_b->setEnabled(true);
    m_ref_one_toggle_facial_landmarks_b->setEnabled(false);
    m_ref_two_toggle_facial_landmarks_b->setEnabled(false);
    m_morph_target_b->setEnabled(false);
    m_target_toggle_facial_landmarks_b->setEnabled(false);

    m_reference_one->reset();
    m_reference_two->reset();
    m_target->reset();
}

/**
 * @brief ImageEditor::morphPressed
 *
 * A private SLOT invoked when the application-user presses the morph
 * button. The method invokes the m_morph_target_b_pressed() method of the
 * EditorPane class.
 *
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
    m_target_toggle_facial_landmarks_b->setEnabled(true);
}

/**
 * @brief ImageEditor::detectLandmarksRefOne
 *
 * A private SLOT invoked when the application-user presses the Detect Landmarks
 * button under the Reference One image. The procedure forwards the request
 * to the EditorPane, in which the image processor resides.
 *
 */
void ImageEditor::detectLandmarksRefOne()
{
    if(m_editor_pane->detectLandmarks(m_reference_one)) {
        m_ref_one_detect_facial_landmarks_b->setEnabled(false);
        m_ref_one_toggle_facial_landmarks_b->setEnabled(true);
    } else {
        m_ref_one_toggle_facial_landmarks_b->setEnabled(false);
    }
}

/**
 * @brief ImageEditor::detectLandmarksRefTwo
 *
 * A private SLOT invoked when the application-user presses the Detect Landmarks
 * button under the Reference Two image. The procedure forwards the request
 * to the EditorPane, in which the image processor resides.
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
 *
 * A private SLOT invoked when the application-user presses the Toggle Landmarks
 * button under the Reference One image. The method toggles the landmark overlay image of the
 * Reference One ImageContainer.
 *
 */
void ImageEditor::toggleLandmarksRefOne()
{
    m_reference_one->toggleLandmarks();
}

/**
 * @brief ImageEditor::toggleLandmarksRefTwo
 *
 * A private SLOT invoked when the application-user presses the Toggle Landmarks
 * button under the Reference Two image. The method toggles the landmark overlay image of the
 * Reference Two ImageContainer.
 *
 */
void ImageEditor::toggleLandmarksRefTwo()
{
    m_reference_two->toggleLandmarks();
}


/**
 * @brief ImageEditor::toggleLandmarksTarget
 *
 * A private SLOT invoked when the application-user presses the Toggle Landmarks
 * button under the Target image. The method toggles the landmark overlay image of the
 * Target ImageContainer.
 *
 */
void ImageEditor::toggleLandmarksTarget()
{
    m_target->toggleLandmarks();
}

/**
 * @brief ImageEditor::testMorphConditions
 *
 * A private method to test if the conditions for image morphing has been satisfied. If the
 * conditions are satisfied, the ImageEditor signals the EditorPane to activate some widgets.
 *
 */
void ImageEditor::testMorphConditions()
{
    if(!m_reference_one->hasImage() || !m_reference_two->hasImage()) return;
    m_morph_target_b->setEnabled(true);
    m_editor_pane->setMorphReady();
}

/**
 * @brief ImageEditor::getEditorPane
 * @return m_editor_pane
 */
EditorPane* ImageEditor::getEditorPane()
{
    return m_editor_pane;
}
