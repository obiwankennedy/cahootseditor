#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QtGui/QDialog>
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
