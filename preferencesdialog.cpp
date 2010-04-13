/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl

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
#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "utilities.h"

#include <QFont>
#include <QFontDialog>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::setMyName(QString name)
{
    myName = name;
    ui->defaultNameLineEdit->setText(name);
}

QString PreferencesDialog::getMyName()
{
    return myName;
}

void PreferencesDialog::setAlwaysUseMyName(bool b)
{
    alwaysUseMyName = b;
    ui->useDefaultNameCheckBox->setChecked(b);
}

bool PreferencesDialog::getAlwaysUseMyName()
{
    return alwaysUseMyName;
}

void PreferencesDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PreferencesDialog::on_changeEditor_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Code Editor Font", QFontDialog::DontUseNativeDialog);
    if (ok) {
        ui->showEditorFont->setFont(newFont);
        ui->showEditorFont->setText("New Font");
        emit editorChangeFont(newFont);
    }
}

void PreferencesDialog::on_changeChat_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Chat Pane Font", QFontDialog::DontUseNativeDialog);
    if (ok) {
        ui->showChatFont->setFont(newFont);
        ui->showChatFont->setText("New Font");
    }
}

void PreferencesDialog::on_changeParticipants_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Participants Pane Font", QFontDialog::DontUseNativeDialog);
    if (ok) {
        ui->showParticipantsFont->setFont(newFont);
        ui->showParticipantsFont->setText("New Font");
    }
}

void PreferencesDialog::on_editorDefault_clicked()
{
    ui->showEditorFont->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize));
    ui->showEditorFont->setText("Default Text");
}

void PreferencesDialog::on_chatDefault_clicked()
{
    ui->showChatFont->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize));
    ui->showChatFont->setText("Default Text");
}

void PreferencesDialog::on_participantsDefault_clicked()
{
    ui->showParticipantsFont->setFont(QFont(Utilities::labelFont, Utilities::labelFontSize));
    ui->showParticipantsFont->setText("Default Text");
}
