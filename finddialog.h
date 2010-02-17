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
    void on_pushButton_findNext_clicked();
    void on_pushButton_findPrev_clicked();

signals:
    void findDialogFindNext(QString str);
    void findDialogFindPrev(QString str);
};

#endif // FINDDIALOG_H
