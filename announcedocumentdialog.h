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
#ifndef ANNOUNCEDOCUMENTDIALOG_H
#define ANNOUNCEDOCUMENTDIALOG_H

#include <QDialog>
#include <QRegExpValidator>

namespace Ui {
    class AnnounceDocumentDialog;
}

class AnnounceDocumentDialog : public QDialog {
    Q_OBJECT
public:
    AnnounceDocumentDialog(QWidget *parent = 0);
    ~AnnounceDocumentDialog();

    bool isBroadcastingChecked();

private:
    Ui::AnnounceDocumentDialog *ui;

    QRegExpValidator *nameValidator;

public slots:
    void setAnnounceDialogInfo(QString name, bool alwaysUseThisName);

private slots:
    void dialogAccepted();

signals:
    void announceDocument(QString name, Qt::CheckState broadcastCheckState, Qt::CheckState alwaysUseChecked);
};

#endif // ANNOUNCEDOCUMENTDIALOG_H
