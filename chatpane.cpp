#include "chatpane.h"
#include "ui_chatpane.h"

#include "utilities.h"

#include <QDebug>
#include <QKeyEvent>
#include <QGridLayout>
#include <QSettings>

ChatPane::ChatPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPane)
{
    ui->setupUi(this);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setVerticalSpacing(0);

    chatBrowser = new ChatBrowser(this);

    layout->addWidget(chatBrowser, 0, 0);
    layout->addWidget(ui->lineEdit, 1, 0);
    this->setLayout(layout);

    connect(chatBrowser, SIGNAL(keyPress(QKeyEvent*)), this, SLOT(textBrowserKeyPress(QKeyEvent*)));
}

ChatPane::~ChatPane()
{
    delete ui;
}

void ChatPane::appendChatMessage(QString str, QColor color)
{
    chatBrowser->addChatLine(str, color);
}

bool ChatPane::hasFocus() const
{
    return ui->lineEdit->hasFocus() || chatBrowser->textCursor().hasSelection();
}

void ChatPane::undo()
{
    if (ui->lineEdit->hasFocus()) {
        ui->lineEdit->undo();
    }
    else if (chatBrowser->hasFocus()) {
        // do nothing, read only
    }
}

void ChatPane::redo()
{
    if (ui->lineEdit->hasFocus()) {
        ui->lineEdit->redo();
    }
    else if (chatBrowser->hasFocus()) {
        // do nothing, read only
    }
}

void ChatPane::cut()
{
    if (ui->lineEdit->hasFocus()) {
        ui->lineEdit->cut();
    }
    else if (chatBrowser->hasFocus()) {
        // do nothing, read only
    }
}

void ChatPane::copy()
{
    if (ui->lineEdit->hasFocus()) {
        ui->lineEdit->copy();
    }
    else {
        chatBrowser->copy();
    }
}

void ChatPane::paste()
{
    ui->lineEdit->paste();
}

void ChatPane::setReadOnly(bool ro)
{
    chatBrowser->setReadOnly(ro);
    ui->lineEdit->setReadOnly(ro);
}

void ChatPane::setFont(QFont font)
{
    chatBrowser->setFont(font);
    ui->lineEdit->setFont(font);
}

void ChatPane::on_lineEdit_returnPressed()
{
    QString sendString = ui->lineEdit->text();
    if (sendString == "") { //etc etc, to be completed
        return;
    }
    ui->lineEdit->clear();

    emit returnPressed(sendString);
}

void ChatPane::textBrowserKeyPress(QKeyEvent *event)
{
    ui->lineEdit->setFocus();
    ui->lineEdit->event(event);
}
