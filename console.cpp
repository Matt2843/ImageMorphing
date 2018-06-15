#include "console.h"

#include <QHBoxLayout>
#include <QFont>

QPlainTextEdit *Console::console = nullptr;
/**
 * @brief Console::Console
 * The default console ctor
 * @param parent
 */
Console::Console(QWidget *parent) :
    QGroupBox("Console", parent)
{
    setup();
}

/**
 * @brief Console::setup
 */
void Console::setup()
{
    QHBoxLayout *m_layout = new QHBoxLayout;
    console = new QPlainTextEdit(this);
    m_layout->addWidget(console);

    console->setReadOnly(true);
    QFont mono("undefined");
    mono.setStyleHint(QFont::Monospace);
    console->setFont(mono);

    setLayout(m_layout);
}

/**
 * @brief Console::appendToConsole
 * @param text
 */
void Console::appendToConsole(const QString &text)
{
    console->appendPlainText(text);
}
