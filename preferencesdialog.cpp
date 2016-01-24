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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "utilities.h"
#include <QFont>
#include <QFontDialog>
#include <QSettings>

#include <QDebug>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    QSettings settings("Cahoots", "Preferences");

    connect(ui->useDefaultNameCheckBox, SIGNAL(clicked()), this, SLOT(storeSharingSettings()));
    connect(ui->defaultNameLineEdit, SIGNAL(textEdited(QString)), this, SLOT(storeName(QString)));
    ui->useDefaultNameCheckBox->setChecked(settings.value("alwaysUseName", false).toBool());
    ui->defaultNameLineEdit->setText(settings.value("myName", "").toString());

    QFont editorFont;
    if (settings.value("editorFont").toString() == "") {
        editorFont = QFont(Utilities::s_codeFont, Utilities::s_codeFontSize);
    }
    else {
        editorFont.fromString(settings.value("editorFont").toString());
    }
    ui->showEditorFont->setFont(editorFont);
    ui->showEditorFont->setText(editorFont.family().toLatin1());

    QFont chatFont;
    if (settings.value("chatFont").toString() == "") {
        chatFont = QFont(Utilities::s_chatFont, Utilities::s_chatFontSize);
    }
    else {
        chatFont.fromString(settings.value("chatFont").toString());
    }
    ui->showChatFont->setFont(chatFont);
    ui->showChatFont->setText(chatFont.family().toLatin1());

    QFont participantsFont;
    if (settings.value("participantsFont").toString() == "") {
        participantsFont = QFont(Utilities::s_labelFont, Utilities::s_labelFontSize);
    }
    else {
        participantsFont.fromString(settings.value("participantsFont").toString());
    }
    ui->showParticipantsFont->setFont(participantsFont);
    ui->showParticipantsFont->setText(participantsFont.family().toLatin1());

    emit setEditorFont(editorFont);
    emit setChatFont(chatFont);
    emit setParticipantsFont(participantsFont);

    ui->changeEditor->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->changeChat->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->changeParticipants->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));

    ui->editorDefault->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->chatDefault->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
    ui->participantsDefault->setFont(QFont(Utilities::s_labelFont, Utilities::s_labelFontSize));
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::setMyName(QString name)
{
    ui->defaultNameLineEdit->setText(name);
    storeSharingSettings();
}

QString PreferencesDialog::getMyName()
{
    return ui->defaultNameLineEdit->text();
    storeSharingSettings();
}

void PreferencesDialog::setAlwaysUseMyName(bool b)
{
    ui->useDefaultNameCheckBox->setChecked(b);
}

bool PreferencesDialog::getAlwaysUseMyName()
{
    return ui->useDefaultNameCheckBox->isChecked();
}

QFont PreferencesDialog::getEditorFont()
{
    return ui->showEditorFont->font();
}

QFont PreferencesDialog::getChatFont()
{
    return ui->showChatFont->font();
}

QFont PreferencesDialog::getParticipantsFont()
{
    return ui->showParticipantsFont->font();
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
    QFont newFont = QFontDialog::getFont(&ok, ui->showEditorFont->font(), this, "Code Editor Font");
    if (ok) {
        QSettings editorFontSettings("Cahoots", "Preferences");
        editorFontSettings.setValue("editorFont", newFont.toString());
        ui->showEditorFont->setFont(newFont);
        ui->showEditorFont->setText(newFont.family().toLatin1());
        emit setEditorFont(newFont);
    }
}

void PreferencesDialog::on_changeChat_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, ui->showChatFont->font(), this, "Chat Pane Font");
    if (ok) {
        QSettings chatFontSettings("Cahoots", "Preferences");
        chatFontSettings.setValue("chatFont", newFont.toString());
        ui->showChatFont->setFont(newFont);
        ui->showChatFont->setText(newFont.family().toLatin1());
        emit setChatFont(newFont);
    }
}

void PreferencesDialog::on_changeParticipants_clicked()
{
    bool ok = false;
    QFont newFont = QFontDialog::getFont(&ok, ui->showParticipantsFont->font(), this, "Participants Pane Font");
    if (ok) {
        QSettings participantsFontSettings("Cahoots", "Preferences");
        participantsFontSettings.setValue("participantsFont", newFont.toString());
        ui->showParticipantsFont->setFont(newFont);
        ui->showParticipantsFont->setText(newFont.family().toLatin1());
        emit setParticipantsFont(newFont);
    }
}

void PreferencesDialog::on_editorDefault_clicked()
{
    QSettings fontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::s_codeFont, Utilities::s_codeFontSize);
    ui->showEditorFont->setFont(defaultFont);
    fontSettings.setValue("editorFont", defaultFont.toString());
    ui->showEditorFont->setText(defaultFont.family().toLatin1());
    emit setEditorFont(defaultFont);
}

void PreferencesDialog::on_chatDefault_clicked()
{
    QSettings fontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::s_chatFont, Utilities::s_chatFontSize);
    ui->showChatFont->setFont(defaultFont);
    fontSettings.setValue("chatFont", defaultFont.toString());
    ui->showChatFont->setText(defaultFont.family().toLatin1());
    emit setChatFont(defaultFont);
}

void PreferencesDialog::on_participantsDefault_clicked()
{
    QSettings fontSettings("Cahoots", "Preferences");
    QFont defaultFont = QFont(Utilities::s_labelFont, Utilities::s_labelFontSize);
    ui->showParticipantsFont->setFont(defaultFont);
    fontSettings.setValue("participantsFont", defaultFont.toString());
    ui->showParticipantsFont->setText(defaultFont.family().toLatin1());
    emit setParticipantsFont(defaultFont);
}

void PreferencesDialog::storeName(QString)
{
    storeSharingSettings();
}

void PreferencesDialog::storeSharingSettings()
{
    QSettings sharingSettings("Cahoots", "Preferences");
    sharingSettings.setValue("alwaysUseName", ui->useDefaultNameCheckBox->isChecked());
    if (ui->defaultNameLineEdit->text() != "") {
        sharingSettings.setValue("myName", ui->defaultNameLineEdit->text());
        qDebug() << "set name: " << ui->defaultNameLineEdit->text();
    }
    emit setAnnounceDialogInfo(ui->defaultNameLineEdit->text(), ui->useDefaultNameCheckBox->isChecked());
}

