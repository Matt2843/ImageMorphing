#pragma once
#include <QGroupBox>

#include <QString>

class QHBoxLayout;
class QPlainTextEdit;
class Console : public QGroupBox
{
    Q_OBJECT
public:
    explicit Console(QWidget * parent = nullptr);
    ~Console() = default;

private:
    void setup();

public:
    static void appendToConsole(const QString &text);
    static void clearConsole();

private:
    QHBoxLayout *m_layout;
    static QPlainTextEdit *console;
};
