#include "document.h"
#include "ui_document.h"

#include "utilities.h"

#include <QDialog>
#include <QKeyEvent>
#include <QTextCursor>
#include <QRegExp>
#include <QDebug>
#include <QWheelEvent>
#include <QTextEdit>
#include <QTextBlock>
#include <QFontMetrics>
#include <QWebView>
#include <QLayout>

#include <QTextDocumentFragment>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    delete ui->frame;
    editor = new CodeEditor(this);
    editor->setFont(QFont("Monaco", 11));
    ui->codeChatSplitter->insertWidget(0, editor);

    cppHighlighter = new CppHighlighter(editor->document());

    connect(editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

//    connect(editor, SIGNAL(blockCountChanged(int)), this, SLOT(onTextChanged(int)));
    connect(editor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChange(int,int,int)));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(on_pushButton_clicked()));

    server = new QTcpServer(this);
    socket = new QTcpSocket(this);

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

    ui->connectInfoLabel->hide();

    newParticipant = true;

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
        socket->connectToHost(QHostAddress(address), port);
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        setChatHidden(false);
        setParticipantsHidden(false);
        ui->connectInfoLabel->show();
        ui->connectInfoLabel->setText(QString("%1:%1").arg(address).arg(port));
    }
}

void Document::undo()
{
    editor->undo();
}

void Document::redo()
{
    editor->redo();
}

void Document::cut()
{
    editor->cut();
}

void Document::copy()
{
    editor->copy();
}

void Document::paste()
{
    editor->paste();
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
    QTextCursor cursor = editor->textCursor();
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
                for (int i = 0; i < 4; i++) {
                    cursor.deleteChar();
                }
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
            for (int i = 0; i < 4; i++) {
                cursor.deleteChar();
            }
        }
        else if (line.startsWith("\t")) {
            cursor.deleteChar();
        }
    }
}

void Document::shiftRight()
{
    QTextCursor cursor = editor->textCursor();
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
    editor->setTextCursor(cursor);
}

void Document::unCommentSelection()
{
    Utilities::unCommentSelection(editor);
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
    return editor->document()->isUndoAvailable();
}

bool Document::isRedoable()
{
    return editor->document()->isRedoAvailable();
}

bool Document::isModified()
{
    return editor->document()->isModified();
}

bool Document::isChatHidden()
{
    return ui->codeChatSplitter->widget(1)->isHidden();
}

bool Document::isParticipantsHidden()
{
    return ui->mainSplitter->widget(1)->isHidden();
}

void Document::findNext(QString str)
{
    editor->find(str, QTextDocument::FindWholeWords);
    editor->setFocus();
}

void Document::findPrev(QString str)
{
    editor->document()->find(str, QTextDocument::FindBackward);
    editor->setFocus();
}

QString Document::getPlainText()
{
    return editor->toPlainText();
}

void Document::setPlainText(QString text)
{
    editor->setPlainText(text);
}

void Document::toggleLineWrap()
{
    if (editor->lineWrapMode() == QPlainTextEdit::NoWrap) {
        editor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }
    else {
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void Document::setModified(bool b)
{
    editor->document()->setModified(b);
}

void Document::previewAsHtml()
{
    QString text = editor->toPlainText();
    QWebView *preview = new QWebView();
    preview->setHtml(text);
    preview->show();
}

void Document::ownerIncomingData(QString data)
{
    return;
    QString toSend;
    if (data.startsWith("doc:")) {
        qDebug() << "odata: " << data;
        toSend = data;
        data.remove(0, 4);
        // detect line number, then put text at that line.
        QRegExp rx = QRegExp("^(\\d+),(\\d+),(\\d+):(.*)");
        if (data.contains(rx)) {
            int pos = rx.cap(1).toInt();
            int charsRemoved = rx.cap(2).toInt();
            int charsAdded = rx.cap(3).toInt();
            QString data = rx.cap(4);
            if (charsRemoved > 0) {
                for (int i = 0; i < charsRemoved; i++) {
                    // Does the charsRemoved indicate characters removed going forward, or back? Needs testing.
                    QTextCursor cursor = editor->textCursor();
                    cursor.setPosition(pos);
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsRemoved);
                    cursor.removeSelectedText();
                }
            }
            else if (charsAdded > 0){
                QTextCursor cursor = editor->textCursor();
                cursor.setPosition(pos);
                cursor.insertText(data.toAscii());
            }
        }
    }
    else {
        toSend = QString("%1: %2").arg(socketToNameMap.value(sender(), "Unknown")).arg(data);
        ui->chatTextEdit->append(toSend);
    }
    // Distribute data to all the other participants

    for (int i = 0; i < clientList.size(); i++) {
        if (clientList.at(i) != sender()) {
            clientList.at(i)->write(toSend.toAscii());
        }
    }

}

void Document::participantIncomingData(QString data)
{
    if (data.startsWith("doc:")) {
        qDebug() << "pdata: " << data;
        data.remove(0, 4);
        // detect line number, then put text at that position.
        QRegExp rx = QRegExp("^(\\d+),(\\d+),(\\d+):(.*)");
        if (data.contains(rx)) {
            int pos = rx.cap(1).toInt();
            int charsRemoved = rx.cap(2).toInt();
            int charsAdded = rx.cap(3).toInt();
            QString data = rx.cap(4);
            if (charsRemoved > 0) {
                for (int i = 0; i < charsRemoved; i++) {
                    // Does the charsRemoved indicate characters removed going forward, or back? Needs testing.
                    QTextCursor cursor = editor->textCursor();
                    cursor.setPosition(pos);
                    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsRemoved);
                    cursor.removeSelectedText();
                }
            }
            else if (charsAdded > 0){
                QTextCursor cursor = editor->textCursor();
                cursor.setPosition(pos);
                cursor.insertText(data.toAscii());
                qDebug() << data;
            }
        }
    }
    else {
        ui->chatTextEdit->append(socket->readAll());
    }

}

void Document::onTextChange(int pos, int charsRemoved, int charsAdded)
{
    QString data;
    if (charsRemoved > 0) {
        data = "";
    }
    else if (charsAdded > 0) {
        QTextCursor cursor = QTextCursor(editor->document());
        cursor.setPosition(pos, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
        data = cursor.selection().toPlainText();
    }

    if (isOwner) {
        QString toSend = QString("doc:%1,%2,%3:%4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data);

        for (int i = 0; i < clientList.size(); i++) {
            clientList.at(i)->write(toSend.toAscii());
        }

    }
    else {
        socket->write(QString("doc:%1,%2,%3:%4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data).toAscii());
    }
}

void Document::on_pushButton_clicked()
{
    QString string = ui->lineEdit->text();
    if (string == "") {
        return;
    }
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

void Document::onIncomingData()
{
    QString data;
    if (isOwner) {
        // We know we have incoming data, so iterate through our current participants to find the
        // correct sender, and then read the data.
        for (int i = 0; i < clientList.size(); i++) {
            if (sender() == clientList.at(i)) {
                // sender() is the sender of the signal that calls this function
                // so we use it to figure out which connection has new data for us
                data = clientList.at(i)->readAll();
                ownerIncomingData(data);
            }
        }
    }
    else { // We are a participant
        data = socket->readAll();
        participantIncomingData(data);
    }
}

void Document::onNewConnection()
{
    qDebug() << "New connection.";
    if (isOwner) {
        clientList.append(server->nextPendingConnection());
        socketToNameMap.insert(clientList.last(), QString("Person %1").arg(clientList.size())); // Empty entry for this connection, to be filled in.
        connect(clientList.last(), SIGNAL(readyRead()), this, SLOT(onIncomingData()));
        ui->noPermissionsListWidget->insertItem(0, QString("Person %1").arg(clientList.size()));
        clientList.last()->write(myName.toAscii());
    }
    else {
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
//        socket->write(myName.toAscii());
    }
}
