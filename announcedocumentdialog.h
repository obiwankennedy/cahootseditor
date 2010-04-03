#ifndef ANNOUNCEDOCUMENTDIALOG_H
#define ANNOUNCEDOCUMENTDIALOG_H

#include <QDialog>

namespace Ui {
    class AnnounceDocumentDialog;
}

class AnnounceDocumentDialog : public QDialog {
    Q_OBJECT
public:
    AnnounceDocumentDialog(QWidget *parent = 0);
    ~AnnounceDocumentDialog();

private:
    Ui::AnnounceDocumentDialog *ui;
};

#endif // ANNOUNCEDOCUMENTDIALOG_H
