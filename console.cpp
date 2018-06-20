#include "console.h"
#include <QDebug>

#include <QFont>
#include <QHBoxLayout>
#include <QPlainTextEdit>

/**
 * @brief Console::console
 *
 * Definition of the static console QPlainTextEdit member.
 *
 */
QPlainTextEdit *Console::console = nullptr;

/**
 * @brief Console::Console
 *
 * The Console ctor setting up the Qt widgets for this
 * GUI container.
 *
 * @param parent the Qt parent container.
 */
Console::Console(QWidget *parent) :
    QGroupBox("Console", parent),
    m_layout(new QHBoxLayout)
{
    setup();
}

/**
 * @brief Console::setup
 *
 * A private convenience method for setting up the layout and
 * internal widgets of this container.
 *
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
 *
 * A public method to append text to the static console
 *
 * @param text
 */
void Console::appendToConsole(const QString &text)
{
    console->appendPlainText(text);
}

/**
 * @brief Console::clearConsole
 *
 * A public method to clear the static console
 *
 */
void Console::clearConsole()
{
    console->clear();
}
