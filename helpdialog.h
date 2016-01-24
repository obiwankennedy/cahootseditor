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
#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "enu.h"

namespace Ui {
    class HelpDialog;
}

class HelpDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(HelpDialog)
public:
    explicit HelpDialog(QWidget *parent = 0);
    virtual ~HelpDialog();

    bool hasFocus();
    // Post: returns true if either of the text edits have focus

    void copy();
    // Post: the text edit that has focus has the copy() operation performed

private:
    Ui::HelpDialog *ui;

    void selectItem(QString string);

private slots:
    void onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*);
};

#endif // HELPDIALOG_H
