#ifndef CONNECTTODOCUMENT_H
#define CONNECTTODOCUMENT_H

#include <QDialog>
#include <QRegExpValidator>

namespace Ui {
    class ConnectToDocument;
}

class ConnectToDocument : public QDialog {
    Q_OBJECT
public:
    ConnectToDocument(QWidget *parent = 0);
    ~ConnectToDocument();

private:
    Ui::ConnectToDocument *ui;

    QRegExpValidator* addressValidator;
    QRegExpValidator* portValidator;

private slots:
    void dialogAccepted();

signals:
    void connectToDocumentClicked(QStringList *list);

};

#endif // CONNECTTODOCUMENT_H
