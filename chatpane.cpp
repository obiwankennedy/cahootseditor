/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
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
