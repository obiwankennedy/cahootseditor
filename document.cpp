#include "document.h"
#include "ui_document.h"

#include <QKeyEvent>
#include <QTextCursor>
#include <QRegExp>
#include <QDebug>
#include <QTextCursor>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    cppHighlighter = new CppHighlighter(ui->codeTextEdit->document());

    connect(ui->codeTextEdit, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(ui->codeTextEdit, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(on_pushButton_clicked()));

    server = new QTcpServer(this);
    socket = new QTcpSocket(this);

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

//    qApp->installEventFilter(this);

    isOwner = true; // We are the document owner, unless we're connecting to someone elses document

}

Document::~Document()
{
    delete ui;
}

void Document::connectToDocument(QStringList *list)
{
    isOwner = false;
    if (list->size() == 2) { // More thorough checking is needed to ensure the contents are usable.
        QString address = list->at(0);
        QString portString = list->at(1);
        int port = portString.toInt();
        qDebug() << "Address: " << address;
        qDebug() << "Port: " << port;
        socket->connectToHost(QHostAddress(address), 3000); // port); // port 3000 magic number for the time being
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        setChatHidden(false);
        setParticipantsHidden(false);
    }
}

void Document::undo()
{
    ui->codeTextEdit->undo();
}

void Document::redo()
{
    ui->codeTextEdit->redo();
}

void Document::cut()
{
    ui->codeTextEdit->cut();
}

void Document::copy()
{
    ui->codeTextEdit->copy();
}

void Document::paste()
{
    ui->codeTextEdit->paste();
}

void Document::find()
{

}

void Document::setParticipantsHidden(bool b)
{
    if (b) {
        ui->mainSplitter->widget(1)->hide();
    }
    else {
        ui->mainSplitter->widget(1)->show();
    }
}

void Document::setChatHidden(bool b)
{
    // Hide/show the widget (contains the chat widget) below the code text edit
    if (b) {
        ui->codeChatSplitter->widget(1)->hide();
    }
    else {
        ui->codeChatSplitter->widget(1)->show();
    }
}

void Document::shiftLeft()
{
    QTextCursor cursor = ui->codeTextEdit->textCursor();
    if (cursor.hasSelection()) {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        cursor.setPosition(start);
        int i = cursor.position();
        int count = 0;
        while (i < (end - (4 * count))) {
            cursor.movePosition(QTextCursor::StartOfLine);
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            QString line = cursor.selectedText();
            cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            if (line.startsWith("    ")) {
                cursor.deleteChar();
                cursor.deleteChar();
                cursor.deleteChar();
                cursor.deleteChar();
            }
            else if (line.startsWith("\t")) {
                cursor.deleteChar();
            }
            cursor.movePosition(QTextCursor::EndOfLine);
            if (cursor.atEnd()) {
                break;
            } else {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::Down);
                i = cursor.position();
            }
            count++;
        }
    } else {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        QString line = cursor.selectedText();
        cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        if (line.startsWith("    ")) {
            cursor.deleteChar();
            cursor.deleteChar();
            cursor.deleteChar();
            cursor.deleteChar();
        }
        else if (line.startsWith("\t")) {
            cursor.deleteChar();
        }
    }
}

void Document::shiftRight()
{
    QTextCursor cursor = ui->codeTextEdit->textCursor();
    if (cursor.hasSelection()) {
        int start = cursor.selectionStart(); //need this so that shift works regardless of direction of selection
        int end = cursor.selectionEnd();
        cursor.setPosition(start);
        int i = cursor.position();
        int count = 0;
        while (i < (end + (4 * count))) {   //4 because that is the number of spaces we add for tab
            cursor.insertText("    ");
            cursor.movePosition(QTextCursor::EndOfLine);
            if (cursor.atEnd()) {
                break;
            } else {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::Down);
                i = cursor.position();
            }
            count++;
        }
    } else {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.insertText("    ");
    }
}

void Document::comment()
{
    QTextCursor cursor = ui->codeTextEdit->textCursor();
    if (cursor.hasSelection()) {
        int start = cursor.selectionStart();
        int end = cursor.selectionEnd();
        cursor.setPosition(start);
        int i = cursor.position();
        int count = 0;
        //while ()                              *NEED TO FIGURE SOMETHING OUT SINCE LINE CAN GET OR LOOSE CHARACTERS...*
    } else {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
        QString line = cursor.selectedText();
        cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
        if (line.startsWith("//")) {
            cursor.deleteChar();
            cursor.deleteChar();
        } else {
            cursor.insertText("//");
        }
    }
}

void Document::announceDocument()
{
    setChatHidden(false);
    setParticipantsHidden(false);

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    server->listen(QHostAddress("137.159.47.71"), 3000);
    ui->chatTextEdit->setText("Listening...");
}

bool Document::isUndoable()
{
    return ui->codeTextEdit->document()->isUndoAvailable();
}

bool Document::isRedoable()
{
    return ui->codeTextEdit->document()->isRedoAvailable();
}

bool Document::isModified()
{
    return ui->codeTextEdit->document()->isModified();
}

bool Document::isChatHidden()
{
    return ui->codeChatSplitter->widget(1)->isHidden();
}

bool Document::isParticipantsHidden()
{
    return ui->mainSplitter->widget(1)->isHidden();
}

QString Document::getPlainText()
{
    return ui->codeTextEdit->toPlainText();
}

void Document::setPlainText(QString text)
{
    ui->codeTextEdit->setPlainText(text);
}

void Document::setModified(bool b)
{
    ui->codeTextEdit->document()->setModified(b);
}

bool Document::eventFilter(QObject *object, QEvent *event)
{
    // filter for keypress events and make sure the source is our text edit.
    if (event->type() == QEvent::KeyPress && object == ui->codeTextEdit) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (!keyEvent->text().isEmpty()) {
//            socket->write(keyEvent->text().toAscii()); // Don't send...yet. Incomplete.
//            qDebug() << keyEvent->text() << " sent.";
        }
    }
    return false;
}

void Document::on_pushButton_clicked()
{
    if (isOwner) {
        QString string = ui->lineEdit->text();
        for (int i = 0; i < clientList.size(); i++) {
            clientList.at(i)->write(QString("Me: ").toAscii() + string.toAscii());
            // "Me" is bad, but we don't have a participants list set up yet. To be changed!
        }
        ui->chatTextEdit->append("Me: " + string);
    }
    else {
        QString string = ui->lineEdit->text();
        socket->write(QString("Them: ").toAscii() + string.toAscii());
        ui->chatTextEdit->append(QString("Me: %1").arg(string));
    }
}

void Document::onIncomingData()
{
    if (isOwner) {
        // We're hosting the document, and are in charge of distributing data.
        QString data;
        // We know we have incoming data, so iterate through our current participants to find the
        // correct sender, and then read the data.
        for (int i = 0; i < clientList.size(); i++) {
            // sender() is the sender of the signal that calls this function
            // so we use it to figure out which connection has new data for us
            if (sender() == clientList.at(i)) {
                data = clientList.at(i)->readAll();
                ui->chatTextEdit->append(QString("Person %1: %2").arg(i).arg(data));
            }
        }
        // Distribute data to all the other participants
        for (int i = 0; i < clientList.size(); i++) {
            if (clientList.at(i) != sender()) {
                clientList.at(i)->write(QString("Person %1: %2").arg(i).arg(data).toAscii());
            }
        }
    }
    else { // We are a participant
        ui->chatTextEdit->append(socket->readAll());
    }
}

void Document::onNewConnection()
{
    qDebug() << "New connection.";
    if (isOwner) {
        clientList.append(server->nextPendingConnection());
        connect(clientList.last(), SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        ui->noPermissionsListWidget->insertItem(0, QString("Person %1").arg(clientList.size()));
    }
    else {
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    }
}

