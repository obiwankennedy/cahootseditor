#ifndef CONNECTTODOCUMENT_H
#define CONNECTTODOCUMENT_H

#include <QDialog>
#include <QRegExpValidator>
#include <QUdpSocket>

#include <QListWidget>

namespace Ui {
    class ConnectToDocument;
}

class ConnectToDocument : public QDialog {
    Q_OBJECT
public:
    ConnectToDocument(QWidget *parent = 0);
    ~ConnectToDocument();

    void setName(QString name);

    QStringList previousInfo; // Public list to store previous names; addresses; ports

private:
    Ui::ConnectToDocument *ui;

    QRegExpValidator* nameValidator;
    QRegExpValidator* addressValidator;
    QRegExpValidator* portValidator;

    QUdpSocket *udpSocket;

    QList<QTimer*> timerList;
    QList<QListWidgetItem*> itemList;

    void addInfo();

    void readSettings();
    void writeSettings();

private slots:
    void dialogAccepted();
    void on_previousDocsComboBox_currentIndexChanged();
    void processPendingDatagrams();
    void timerTimedOut();
    void listWidgetItemClicked(QListWidgetItem *current);

signals:
    void connectToDocumentClicked(QStringList list);

};

#endif // CONNECTTODOCUMENT_H
