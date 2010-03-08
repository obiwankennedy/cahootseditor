#include "chatpane.h"
#include "ui_chatpane.h"

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
        ui->lineEdit->copy();
    }
    else if (ui->textBrowser->hasFocus()) {
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

