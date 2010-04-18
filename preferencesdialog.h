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
#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QSettings>

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog {
    Q_OBJECT
public:
    PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    void setMyName(QString name);
    QString getMyName();
    void setAlwaysUseMyName(bool b);
    bool getAlwaysUseMyName();

    QFont getEditorFont();
    QFont getChatFont();
    QFont getParticipantsFont();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PreferencesDialog *ui;

private slots:
    void on_changeEditor_clicked();
    void on_changeChat_clicked();
    void on_changeParticipants_clicked();
    void on_editorDefault_clicked();
    void on_chatDefault_clicked();
    void on_participantsDefault_clicked();

    void storeSharingSettings();

signals:
    void setEditorFont(QFont);
    void setChatFont(QFont);
    void setParticipantsFont(QFont);
    void setAnnounceDialogInfo(QString, bool);
};

#endif // PREFERENCESDIALOG_H
