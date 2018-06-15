#pragma once
#include <QGroupBox>

#include <QString>
#include <QPlainTextEdit>

class Console : public QGroupBox
{
    Q_OBJECT
public:
    Console(QWidget * parent = nullptr);

private:
    void setup();

public:
    static void appendToConsole(const QString &text);

private:
    static QPlainTextEdit *console;
};
