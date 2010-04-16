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
