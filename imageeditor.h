#pragma once
#include <QGroupBox>

#include "imagecontainer.h"
#include "editorpane.h"

#include <memory>

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ImageEditor : public QGroupBox {
    Q_OBJECT

public:
    explicit ImageEditor(QWidget * parent = nullptr);
    ~ImageEditor() = default;

private:
    void setup();

public:
    void setReferenceOne(ImageContainer * ref_one);
    void setReferenceTwo(ImageContainer * ref_two);

private:
    std::unique_ptr<QVBoxLayout> m_layout;

    std::unique_ptr<ImageContainer> m_reference_one;
    std::unique_ptr<ImageContainer> m_reference_two;
    std::unique_ptr<ImageContainer> m_target;

    std::unique_ptr<QHBoxLayout> m_containers_layout;
    std::unique_ptr<QHBoxLayout> m_label_layout;

    std::unique_ptr<QLabel> m_ref_one_label;
    std::unique_ptr<QLabel> m_target_label;
    std::unique_ptr<QLabel> m_ref_two_label;

    std::unique_ptr<EditorPane> m_editor_pane;
};
