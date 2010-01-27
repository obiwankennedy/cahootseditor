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
    Ui::FindDialog *m_ui;
};

#endif // FINDDIALOG_H
