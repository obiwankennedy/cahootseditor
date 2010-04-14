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

protected:
    void changeEvent(QEvent *e);

private:
    Ui::PreferencesDialog *ui;

    QString myName;
    bool alwaysUseMyName;

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
};

#endif // PREFERENCESDIALOG_H
