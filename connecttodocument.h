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

    void setName(QString name);

    QStringList previousInfo; //Global list to store previous names; addresses; ports
    void initializeCombobox(QStringList list);

private:

    Ui::ConnectToDocument *ui;

    QRegExpValidator* addressValidator;
    QRegExpValidator* portValidator;

    void addInfo();

private slots:
    void dialogAccepted();
    void on_previousDocsComboBox_currentIndexChanged();

signals:
    void connectToDocumentClicked(QStringList *list);

};

#endif // CONNECTTODOCUMENT_H
