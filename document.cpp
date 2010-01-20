#include "document.h"
#include "ui_document.h"

#include <QKeyEvent>
#include <QDebug>

Document::Document(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::Document)
{
    m_ui->setupUi(this);

    cppHighlighter = new CppHighlighter(m_ui->codeTextEdit->document());

    connect(m_ui->codeTextEdit, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(m_ui->codeTextEdit, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

//    m_ui->listWidget->hide();

    server = new QTcpServer(this);
    socket = new QTcpSocket(this);

    qApp->installEventFilter(this);
}

Document::~Document()
{
    delete m_ui;
}

void Document::undo()
{
    m_ui->codeTextEdit->undo();
}

void Document::redo()
{
    m_ui->codeTextEdit->redo();
}

void Document::cut()
{
    m_ui->codeTextEdit->cut();
}

void Document::copy()
{
    m_ui->codeTextEdit->copy();
}

void Document::paste()
{
    m_ui->codeTextEdit->paste();
}

bool Document::isUndoable()
{
    return m_ui->codeTextEdit->document()->isUndoAvailable();
}

bool Document::isRedoable()
{
    return m_ui->codeTextEdit->document()->isRedoAvailable();
}

bool Document::isModified()
{
    return m_ui->codeTextEdit->document()->isModified();
}

QString Document::getPlainText()
{
    return m_ui->codeTextEdit->toPlainText();
}

void Document::setPlainText(QString text)
{
    m_ui->codeTextEdit->setPlainText(text);
}

void Document::setModified(bool b)
{
    m_ui->codeTextEdit->document()->setModified(b);
}

bool Document::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && object == m_ui->codeTextEdit) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (!keyEvent->text().isEmpty()) {
            socket->write(keyEvent->text().toAscii());
            qDebug() << keyEvent->text() << " sent.";
        }
    }
    return false;
}

