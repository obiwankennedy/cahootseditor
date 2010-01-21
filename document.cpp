#include "document.h"
#include "ui_document.h"

#include <QKeyEvent>
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

    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

    server->listen(QHostAddress("137.159.47.71"), 3000);
    ui->chatTextEdit->setText("Listening...");

//    qApp->installEventFilter(this);
}

Document::~Document()
{
    delete ui;
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
    return ui->chatTextEdit->isHidden();
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

void Document::onNewConnection()
{
//    ui->statusBar->showMessage("We have signal! Main screen turn on!");

//    clientList.append(server->nextPendingConnection());
//    connect(clientList.last(), SIGNAL(readyRead()), this, SLOT(onIncomingData()));
}

void Document::on_pushButton_clicked()
{
//    QString string = ui->textEdit->toPlainText();
//    sock->write(string.toAscii());
//    ui->textEdit->append(QString("Sent \"%1\"\n").arg(string));
}
