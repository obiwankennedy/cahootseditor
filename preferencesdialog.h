#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
    class PreferencesDialog;
}

class PreferencesDialog : public QDialog {
    Q_OBJECT
public:
    PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

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

signals:
    void preferencesClicked();
};

#endif // PREFERENCESDIALOG_H
