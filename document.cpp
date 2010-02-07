#include "document.h"
#include "ui_document.h"

#include <QKeyEvent>
#include <QTextCursor>
#include <QRegExp>
#include <QDebug>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    cppHighlighter = new CppHighlighter(ui->codeTextEdit->document());

    connect(ui->codeTextEdit, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(ui->codeTextEdit, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(on_pushButton_clicked()));

    connect(ui->codeTextEdit->document(), SIGNAL(blockCountChanged(int)), this, SLOT(codeTextEditTextChanged(int)));

    server = new QTcpServer(this);
    socket = new QTcpSocket(this);

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

    ui->connectInfoLabel->hide();

//    qApp->installEventFilter(this);

    myName = "Owner"; // temporary

    isOwner = true; // We are the document owner, unless we're connecting to someone elses document

}

Document::~Document()
{
    delete ui;
}

void Document::connectToDocument(QStringList *list)
{
    isOwner = false;
    if (list->size() == 3) { // More thorough checking is needed to ensure the contents are usable.
        myName = list->at(0);
        QString address = list->at(1);
        QString portString = list->at(2);
        int port = portString.toInt();
        qDebug() << "Address: " << address;
        qDebug() << "Port: " << port;
        socket->connectToHost(QHostAddress(address), 3000); // port); // port 3000 magic number for the time being
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        setChatHidden(false);
        setParticipantsHidden(false);
        ui->connectInfoLabel->show();
        ui->connectInfoLabel->setText(QString("%1:%1").arg(address).arg(port));
    }
}

void Document::undo()
{
//    QTextCursor cursor = ui->codeTextEdit->textCursor();
//    if (cursor.hasSelection()) {
//        int start = cursor.selectionStart();
//        cursor.setPosition(cursor.selectionEnd());
//        cursor.movePosition(QTextCursor::StartOfLine);
//        int end = cursor.position();
//        ui->codeTextEdit->undo();
//        ui->codeTextEdit->setTextCursor(cursor);
//        cursor.setPosition(start);
//        cursor.setPosition(end, QTextCursor::KeepAnchor);
//        //cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
//        ui->codeTextEdit->setTextCursor(cursor);
//    } else {
       ui->codeTextEdit->undo();
//    }
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
        cursor.beginEditBlock();
        while ( i < end) {
            cursor.movePosition(QTextCursor::StartOfLine);
            cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
            QString line = cursor.selectedText();
            cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
            if (line.startsWith("    ")) {
                cursor.deleteChar();
                cursor.deleteChar();
                cursor.deleteChar();
                cursor.deleteChar();
                end -= 4;
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
        }
        cursor.endEditBlock();
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
    int end = cursor.selectionEnd();
    int start = cursor.selectionStart();
    if (cursor.hasSelection()) {
        cursor.setPosition(start);
        int i = cursor.position();
        cursor.beginEditBlock();
        while (i < end) {
            cursor.insertText("    ");
            end += 4;
            cursor.movePosition(QTextCursor::EndOfLine);
            if (cursor.atEnd()) {
                break;
            } else {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::Down);
                i = cursor.position();
            }
        }
        cursor.endEditBlock();
    } else {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.insertText("    ");
    }
    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    ui->codeTextEdit->setTextCursor(cursor);
}

void Document::comment()
{
    QTextCursor cursor = ui->codeTextEdit->textCursor();
    int start = cursor.selectionStart();
    int end = cursor.selectionEnd();
    if (cursor.hasSelection()) {
        QString line = cursor.selectedText();
        cursor.setPosition(start);
        if (!cursor.atBlockStart()) {
            if (line.startsWith("/*") && line.endsWith("*/")) {
                cursor.beginEditBlock();
                cursor.deleteChar();
                cursor.deleteChar();
                end -= 2;
                cursor.setPosition(end);
                cursor.deletePreviousChar();
                cursor.deletePreviousChar();
                end -= 2;
                cursor.endEditBlock();
            } else {
                cursor.beginEditBlock();
                cursor.setPosition(start);
                cursor.insertText("/*");
                end += 2;
                cursor.setPosition(end);
                cursor.insertText("*/");
                end += 2;
                cursor.endEditBlock();
            }
        } else {
            int i = cursor.position();
            //int count = 0;
            bool isCommented = true;
            while (i < end) {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                QString line = cursor.selectedText();
                cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                if (!(line.startsWith("//") && isCommented)) {
                    isCommented = false;
                }
                cursor.movePosition(QTextCursor::EndOfLine);
                if (cursor.atEnd()) {
                    break;
                } else {
                    cursor.movePosition(QTextCursor::StartOfLine);
                    cursor.movePosition(QTextCursor::Down);
                    i = cursor.position();
                }
            }
            cursor.setPosition(start);
            i = cursor.position();
            cursor.beginEditBlock();
            while (i < end) {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
                QString line = cursor.selectedText();
                cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
                if (isCommented) {
                    cursor.deleteChar();
                    cursor.deleteChar();
                    end -= 2;
                } else {
                    cursor.insertText("//");
                    end += 2;
                }
                cursor.movePosition(QTextCursor::EndOfLine);
                if (cursor.atEnd()) {
                    break;
                } else {
                    cursor.movePosition(QTextCursor::StartOfLine);
                    cursor.movePosition(QTextCursor::Down);
                    i = cursor.position();
                }
            }
            cursor.endEditBlock();
        }
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

    cursor.setPosition(start);
    cursor.setPosition(end, QTextCursor::KeepAnchor);
    ui->codeTextEdit->setTextCursor(cursor);
}

void Document::announceDocument()
{
    setChatHidden(false);
    setParticipantsHidden(false);

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    server->listen(QHostAddress::Any, 0); // Port is chosen automatically, listening on all NICs
    ui->chatTextEdit->setText("Listening...");

    ui->connectInfoLabel->show();
    QString port = QString::number(server->serverPort(), 10);
    ui->connectInfoLabel->setText(port);

    ui->readWriteListWidget->insertItem(0, myName);
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

void Document::toggleLineWrap()
{
    if (ui->codeTextEdit->lineWrapMode() == QTextEdit::NoWrap) {
        ui->codeTextEdit->setLineWrapMode(QTextEdit::WidgetWidth);
    }
    else {
        ui->codeTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    }
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
    QString string = ui->lineEdit->text();
    if (isOwner) {
        for (int i = 0; i < clientList.size(); i++) {
            clientList.at(i)->write(QString(myName + ": ").toAscii() + string.toAscii());
        }
    }
    else {
        socket->write(string.toAscii());
    }
    ui->chatTextEdit->append(myName + ": " + string);
    ui->lineEdit->clear();
}

void Document::codeTextEditTextChanged(int lineCount)
{
    qDebug() << "Line count: " << lineCount;
    QString string = "";
    for (int i = 1; i <= lineCount; i++) {
        string.append(QString("%1\n").arg(i));
    }
    ui->lineNumberTextEdit->setText(string);
}

void Document::onIncomingData()
{
    if (isOwner) {
        // We're hosting the document, and are in charge of distributing data.
        QString data;
        bool newParticipant = false;
        // We know we have incoming data, so iterate through our current participants to find the
        // correct sender, and then read the data.
        for (int i = 0; i < clientList.size(); i++) {
            // sender() is the sender of the signal that calls this function
            // so we use it to figure out which connection has new data for us
            if (sender() == clientList.at(i)) {
                data = clientList.at(i)->readAll();
                if (socketToNameMap.value(sender()) == "") {
                    socketToNameMap.insert(sender(), data);
                    newParticipant = true;
                }
                else {
                    ui->chatTextEdit->append(QString("%1: %2").arg(socketToNameMap.value(sender(), "Unknown")).arg(data));
                }
            }
        }
        // Distribute data to all the other participants
        for (int i = 0; i < clientList.size(); i++) {
            if (clientList.at(i) != sender() && !newParticipant) {
                clientList.at(i)->write(QString("%1: %2").arg(socketToNameMap.value(sender(), "Unknown")).arg(data).toAscii());
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
        socketToNameMap.insert(clientList.last(), ""); // Empty entry for this connection, to be filled in.
        connect(clientList.last(), SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        ui->noPermissionsListWidget->insertItem(0, QString("Person %1").arg(clientList.size()));
    }
    else {
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        qApp->processEvents();
        socket->write(myName.toAscii());
    }
}

