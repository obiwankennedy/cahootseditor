#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class FindDialog;
}

class FindDialog : public QDialog {
    Q_OBJECT
public:
    FindDialog(QWidget *parent = 0);
    ~FindDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FindDialog *ui;

private slots:
    void on_findNextPushButton_clicked();
    void on_findPreviousPushButton_clicked();

signals:
    void findDialogFindNext(QString str, bool ignoreCase, bool wrapAround);
    void findDialogFindPrev(QString str, bool ignoreCase, bool wrapAround);
};

#endif // FINDDIALOG_H
