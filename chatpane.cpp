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

void ChatPane::on_lineEdit_returnPressed()
{
    QString sendString = ui->lineEdit->text();
    if (sendString == "") { //etc etc, to be completed
        return;
    }
    ui->lineEdit->clear();

    emit returnPressed(sendString);
}

