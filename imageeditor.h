#pragma once
#include <QGroupBox>

#include "imagecontainer.h"
#include "editorpane.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ImageEditor : public QGroupBox {
    Q_OBJECT

public:
    explicit ImageEditor(QWidget *parent = nullptr);
    ~ImageEditor() = default;

private:
    void setup();

public:
    void setReferenceOne(ImageContainer *ref_one);
    void setReferenceTwo(ImageContainer *ref_two);

public slots:
    void attemptToSetReferenceByDoubleClick(ImageContainer *img);
    void attemptToSetReference(ImageContainer *img, int number);

private:
    QVBoxLayout *m_layout;

    ImageContainer *m_reference_one;
    ImageContainer *m_reference_two;
    ImageContainer *m_target;

    QHBoxLayout *m_containers_layout;
    QHBoxLayout *m_label_layout;

    QLabel *m_ref_one_label;
    QLabel *m_target_label;
    QLabel *m_ref_two_label;

    EditorPane *m_editor_pane;
};
