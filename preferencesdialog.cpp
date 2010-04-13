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
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    QSettings fontSettings("Cahoots", "PreferencesPane");
    QFont editorFont;
    if (fontSettings.value("editorFont").toString() == "") {
        editorFont = QFont(Utilities::codeFont, Utilities::codeFontSize);
    }
    else {
        editorFont.fromString(fontSettings.value("editorFont").toString());
    }
    ui->showEditorFont->setFont(editorFont);
    ui->showEditorFont->setText(editorFont.family().toAscii());

    QFont chatFont;
    if (fontSettings.value("chatFont").toString() == "") {
        chatFont = QFont(Utilities::chatFont, Utilities::chatFontSize);
    }
    else {
        chatFont.fromString(fontSettings.value("chatFont").toString());
    }
    ui->showChatFont->setFont(chatFont);
    ui->showChatFont->setText(chatFont.family().toAscii());

    QFont participantsFont;
    if (fontSettings.value("participantsFont").toString() == "") {
        participantsFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    }
    else {
        participantsFont.fromString(fontSettings.value("participantsFont").toString());
    }
    ui->showParticipantsFont->setFont(participantsFont);
    ui->showParticipantsFont->setText(participantsFont.family().toAscii());

    emit setEditorFont(editorFont);
    emit setChatFont(chatFont);
    emit setParticipantsFont(participantsFont);
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
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Code Editor Font");
    if (ok) {
        QSettings editorFontSettings("Cahoots", "Preferences");
        editorFontSettings.setValue("editorFont", newFont.toString());
        ui->showEditorFont->setFont(newFont);
        ui->showEditorFont->setText(newFont.family().toAscii());
        emit setEditorFont(newFont);
    }
}

void PreferencesDialog::on_changeChat_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Chat Pane Font");
    if (ok) {
        QSettings chatFontSettings("Cahoots", "Preferences");
        chatFontSettings.setValue("chatFont", newFont.toString());
        ui->showChatFont->setFont(newFont);
        ui->showChatFont->setText(newFont.family().toAscii());
        emit setChatFont(newFont);
    }
}

void PreferencesDialog::on_changeParticipants_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Participants Pane Font");
    if (ok) {
        QSettings participantsFontSettings("Cahoots", "Preferences");
        participantsFontSettings.setValue("participantsFont", newFont.toString());
        ui->showParticipantsFont->setFont(newFont);
        ui->showParticipantsFont->setText(newFont.family().toAscii());
        emit setParticipantsFont(newFont);
    }
}

void PreferencesDialog::on_editorDefault_clicked()
{
    QSettings fontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    ui->showEditorFont->setFont(defaultFont);
    fontSettings.setValue("editorFont", defaultFont.toString());
    ui->showEditorFont->setText(defaultFont.family().toAscii());
    emit setEditorFont(defaultFont);
}

void PreferencesDialog::on_chatDefault_clicked()
{
    QSettings fontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    ui->showEditorFont->setFont(defaultFont);
    fontSettings.setValue("chatFont", defaultFont.toString());
    ui->showChatFont->setText(defaultFont.family().toAscii());
    emit setChatFont(defaultFont);
}

void PreferencesDialog::on_participantsDefault_clicked()
{
    QSettings fontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    ui->showEditorFont->setFont(defaultFont);
    fontSettings.setValue("participantsFont", defaultFont.toString());
    ui->showParticipantsFont->setText(defaultFont.family().toAscii());
    emit setParticipantsFont(defaultFont);
}
