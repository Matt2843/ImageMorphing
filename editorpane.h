#pragma once

#include <QGroupBox>

class EditorPane : public QGroupBox {
    Q_OBJECT
public:
    explicit EditorPane(QWidget * parent = nullptr);
    ~EditorPane() = default;

};
