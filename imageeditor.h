#pragma once
#include <QGroupBox>

class EditorPane;
class ImageContainer;
class Console;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class ImageEditor : public QGroupBox
{
    Q_OBJECT

public:
    explicit ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor() = default;

private:
    void setup();
    void setupConnections();

public:
    void setReferenceOne(ImageContainer *ref_one);
    void setReferenceTwo(ImageContainer *ref_two);

public slots:
    void attemptToSetReferenceByDoubleClick(ImageContainer *img);
    void attemptToSetReference(ImageContainer *img, int number);
    void resetAll();

private slots:
    void morphPressed();
    void detectLandmarksRefOne();
    void detectLandmarksRefTwo();
    void toggleLandmarksRefOne();
    void toggleLandmarksRefTwo();
    void toggleLandmarksTarget();

private:
    void testMorphConditions();

public:
    EditorPane* getEditorPane();

private:
    QVBoxLayout *m_layout;

    ImageContainer *m_reference_one;
    ImageContainer *m_reference_two;
    ImageContainer *m_target;

    QHBoxLayout *m_ref_one_buttons;
    QHBoxLayout *m_ref_two_buttons;
    QHBoxLayout *m_target_buttons;

    QVBoxLayout *m_reference_one_group;
    QVBoxLayout *m_reference_two_group;
    QVBoxLayout *m_target_group;

    QPushButton *m_ref_one_detect_facial_landmarks_b;
    QPushButton *m_ref_one_toggle_facial_landmarks_b;
    QPushButton *m_ref_two_detect_facial_landmarks_b;
    QPushButton *m_ref_two_toggle_facial_landmarks_b;
    QPushButton *m_morph_target_b;
    QPushButton *m_target_toggle_facial_landmarks_b;

    QHBoxLayout *m_containers_layout;

    QLabel *m_ref_one_label;
    QLabel *m_target_label;
    QLabel *m_ref_two_label;

    EditorPane *m_editor_pane;
    Console *console;
};
