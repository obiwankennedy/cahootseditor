#include "chatpane.h"
#include "ui_chatpane.h"

#include <QDebug>

ChatPane::ChatPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatPane)
{
    ui->setupUi(this);
}

ChatPane::~ChatPane()
{
    delete ui;
}

void ChatPane::appendChatMessage(QString str)
{
    ui->textBrowser->append(str);
}

bool ChatPane::hasFocus() const
{
    return ui->lineEdit->hasFocus() || ui->textBrowser->textCursor().hasSelection();
}
void ChatPane::undo()
{
    if (ui->lineEdit->hasFocus()) {
        ui->lineEdit->undo();
    }
    else if (ui->textBrowser->hasFocus()) {
        // do nothing, read only
    }

}

void ChatPane::redo()
{
    if (ui->lineEdit->hasFocus()) {
        ui->lineEdit->redo();
    }
    else if (ui->textBrowser->hasFocus()) {
        // do nothing, read only
    }
}

void ChatPane::cut()
{
    if (ui->lineEdit->hasFocus()) {
        ui->lineEdit->cut();
    }
    else if (ui->textBrowser->hasFocus()) {
        // do nothing, read only
    }
}

void ChatPane::copy()
{
    if (ui->lineEdit->hasFocus()) {
        qDebug() << "lineEdit has focus";
        ui->lineEdit->copy();
    }
    else {
        ui->textBrowser->copy();
    }
}

void ChatPane::paste()
{
    ui->lineEdit->paste();
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

