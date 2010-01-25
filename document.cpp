#include "document.h"
#include "ui_document.h"

#include <QKeyEvent>
#include <QTextCursor>
#include <QDebug>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    cppHighlighter = new CppHighlighter(ui->codeTextEdit->document());

    connect(ui->codeTextEdit, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(ui->codeTextEdit, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    server = new QTcpServer(this);
    socket = new QTcpSocket(this);

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

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
    if (list->size() == 2) {
        QString address = list->at(0);
        QString portString = list->at(1);
        int port = portString.toInt();
        socket->connectToHost(QHostAddress(address), port);
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

void Document::setParticipantsHidden(bool b)
{
    if (b) {
        ui->listWidget->hide();
    }
    else {
        ui->listWidget->show();
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
    QTextCursor *cursor = new QTextCursor(ui->codeTextEdit->document());
    int currentPosition = cursor->position();
    cursor->setPosition(0, QTextCursor::MoveAnchor); // This only affects the first line of the document
    cursor->insertText("    ");
    cursor->setPosition(currentPosition, QTextCursor::MoveAnchor);
}

void Document::shiftRight()
{
    QTextCursor cursor = ui->codeTextEdit->textCursor();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText("    ");
}

void Document::announceDocument()
{
    setChatHidden(false);
    setParticipantsHidden(false);
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
    return ui->listWidget->isHidden();
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
    QString string = ui->lineEdit->text();
    socket->write(QString("Them: ").toAscii() + string.toAscii());
    ui->chatTextEdit->append(QString("Me: %1\n").arg(string));
}

void Document::onIncomingData()
{
    if (isOwner) { // We're hosting the document, and are in charge of distributing data.
        QString data;
        qDebug() << "sender() == " << sender();
        // We know we have incoming data, so iterate through our current participants to find the
        // correct sender, and then read the data.
        for (int i = 0; i < clientList.size(); i++) {
            qDebug() << "clientList.at(" << i << ") = " << clientList.at(i);
            if (sender() == clientList.at(i)) {
                data = clientList.at(i)->readAll();
                ui->chatTextEdit->insertPlainText(data);
                #warning: To do: distribute data to the rest of the participants
            }
        }
    }
    else { // We are a participant
        ui->chatTextEdit->append(socket->readAll());
    }
}

void Document::onNewConnection()
{
    if (isOwner) {
        clientList.append(server->nextPendingConnection());
        connect(clientList.last(), SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    }
    else {
        connect(socket, SIGNAL(readyRead()), this, SLOT(onIncomingData()));
    }
}

