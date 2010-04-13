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

    QSettings editorFontSettings("Cahoots", "Preferences");
    QFont editorFont;
    if (editorFontSettings.value("editorFont").toString() == "") {
        editorFont = QFont(Utilities::codeFont, Utilities::codeFontSize);
    }
    else {
        bool ok = editorFont.fromString(editorFontSettings.value("editorFont").toString());
    }
    ui->showEditorFont->setFont(editorFont);
    ui->showEditorFont->setText("Current Font");

    QSettings chatFontSettings("Cahoots", "Preferences");
    QFont chatFont;
    if (chatFontSettings.value("chatFont").toString() == "") {
        chatFont = QFont(Utilities::chatFont, Utilities::chatFontSize);
    }
    else {
        bool ok = chatFont.fromString(editorFontSettings.value("chatFont").toString());
    }
    ui->showChatFont->setFont(chatFont);
    ui->showChatFont->setText("Current Font");

    QSettings participantsFontSettings ("Cahoots", "Preferences");
    QFont participantsFont;
    if (participantsFontSettings.value("participantsFont").toString() == "") {
        participantsFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    }
    else {
        bool ok = participantsFont.fromString(editorFontSettings.value("participantsFont").toString());
    }
    ui->showParticipantsFont->setFont(participantsFont);
    ui->showParticipantsFont->setText("Current Font");
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
        QSettings editorFontSettings("Cahoots", "Preferences");
        editorFontSettings.setValue("editorFont", newFont.toString());
        ui->showEditorFont->setFont(newFont);
        ui->showEditorFont->setText("New Font");
        emit editorChangeFont(editorFontSettings.value("Font").toString());
    }
}

void PreferencesDialog::on_changeChat_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Chat Pane Font", QFontDialog::DontUseNativeDialog);
    if (ok) {
        QSettings chatFontSettings("Cahoots", "Preferences");
        chatFontSettings.setValue("chatFont", newFont.toString());
        ui->showChatFont->setFont(newFont);
        ui->showChatFont->setText("New Font");
        emit chatChangeFont(chatFontSettings.value("Font").toString());
    }
}

void PreferencesDialog::on_changeParticipants_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, QFont(font()), this, "Participants Pane Font", QFontDialog::DontUseNativeDialog);
    if (ok) {
        QSettings participantsFontSettings("Cahoots", "Preferences");
        participantsFontSettings.setValue("participantsFont", newFont.toString());
        ui->showParticipantsFont->setFont(newFont);
        ui->showParticipantsFont->setText("New Font");
        emit participantsChangeFont(participantsFontSettings.value("Font").toString());
    }
}

void PreferencesDialog::on_editorDefault_clicked()
{
    QSettings editorFontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    ui->showEditorFont->setFont(defaultFont);
    editorFontSettings.setValue("editorFont", defaultFont.toString());
    ui->showEditorFont->setText("Default Text");
    emit editorChangeFont(editorFontSettings.value("Font").toString());
}

void PreferencesDialog::on_chatDefault_clicked()
{
    QSettings chatFontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    ui->showEditorFont->setFont(defaultFont);
    chatFontSettings.setValue("chatFont", defaultFont.toString());
    ui->showChatFont->setText("Default Text");
    emit chatChangeFont(chatFontSettings.value("Font").toString());
}

void PreferencesDialog::on_participantsDefault_clicked()
{
    QSettings participantsFontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::labelFont, Utilities::labelFontSize);
    ui->showEditorFont->setFont(defaultFont);
    participantsFontSettings.setValue("participantsFont", defaultFont.toString());
    ui->showParticipantsFont->setText("Default Text");
    emit participantsChangeFont(participantsFontSettings.value("Font").toString());
}
