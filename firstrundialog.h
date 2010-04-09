#ifndef FIRSTRUNDIALOG_H
#define FIRSTRUNDIALOG_H

#include <QDialog>

namespace Ui {
    class FirstRunDialog;
}

class FirstRunDialog : public QDialog {
    Q_OBJECT
public:
    FirstRunDialog(QWidget *parent = 0);
    ~FirstRunDialog();

private:
    Ui::FirstRunDialog *ui;

private slots:
    void dialogAccepted();
};

#endif // FIRSTRUNDIALOG_H
