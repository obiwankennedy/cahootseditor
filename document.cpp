#include "document.h"
#include "ui_document.h"

#include "utilities.h"
#include "enu.h"

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
#include <QMessageBox>
#include <QStringList>

#include <QTextDocumentFragment>

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    ui->bottomEditorFrame->hide();

    delete ui->editorFrame;
    editor = new CodeEditor(this);
    editor->setFont(QFont(Utilities::codeFont, Utilities::codeFontSize));
    QFontMetrics fm(editor->font());
    editor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(0, editor);
    
    // for split pane editing
    delete ui->bottomEditorFrame;
    bottomEditor = new CodeEditor(this);
    bottomEditor->setFont(editor->font());
    bottomEditor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(1, bottomEditor);
    // we don't need the default document since we're using the document of the original editor
    bottomEditor->document()->deleteLater();
    bottomEditor->setDocument(editor->document());
    bottomEditor->hide();

    highlighter = new CppHighlighter(editor->document());

    delete ui->participantFrame;
    participantPane = new ParticipantsPane();
    ui->participantSplitter->insertWidget(1, participantPane);

    delete ui->chatFrame;
    chatPane = new ChatPane();
    ui->codeChatSplitter->insertWidget(1, chatPane);

    connect(editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

    myName = "Owner"; // temporary

    isAlreadyAnnounced = false;
    myPermissions = Enu::ReadWrite;
}

Document::~Document()
{
    delete ui;
}

void Document::announceDocument()
{
    isAlreadyAnnounced = true;
    setChatHidden(false);
    setParticipantsHidden(false);

    server = new Server(editor, participantPane, chatPane);

    server->listen(QHostAddress::Any, 0); // Port is chosen automatically, listening on all NICs
    QString port = QString::number(server->serverPort(), 10);

    chatPane->appendChatMessage("Listening on port " + port);

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address())
            ipAddress = ipAddressesList.at(i).toString();
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty()) {
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    }

    participantPane->setConnectInfo(ipAddress + ":" + port);
}

void Document::connectToDocument(QStringList list)
{
    myPermissions = Enu::Waiting;

    myName = list.at(0);    
    QString address = list.at(1);
    QString portString = list.at(2);

    int port = portString.toInt();
    participantPane->setOwnership(false);
    isAlreadyAnnounced = true;

    setChatHidden(false);
    setParticipantsHidden(false);
    editor->setEnabled(false);

    client = new Client(editor, participantPane, chatPane);
    client->setUsername(myName);
    client->connectToHost(QHostAddress(address), port);
    participantPane->setConnectInfo(QString("%1:%2").arg(address).arg(portString));
}

void Document::undo()
{
    if (editor->hasFocus()) {
        editor->undo();
    }
    else if (chatPane->hasFocus()) {
        chatPane->undo();
    }
}

void Document::redo()
{
    if (editor->hasFocus()) {
        editor->redo();
    }
    else if (chatPane->hasFocus()) {
        chatPane->redo();
    }
}

void Document::cut()
{
    if (editor->hasFocus()) {
        editor->cut();
    }
    else if (participantPane->hasFocus()){
        // do nothing
    }
    else if (chatPane->hasFocus()) {
        chatPane->cut();
    }
}

void Document::copy()
{
    if (editor->hasFocus()) {
        editor->copy();
    }
    else if (participantPane->hasFocus()){
        // do nothing
    }
    else if (chatPane->hasFocus()) {
        chatPane->copy();
    }
}

void Document::paste()
{
    if (editor->hasFocus()) {
        editor->paste();
    }
    else if (participantPane->hasFocus()){
        // do nothing
    }
    else if (chatPane->hasFocus()) {
        chatPane->paste();
    }
}

void Document::setParticipantsHidden(bool b)
{
    if (b) {
        ui->participantSplitter->widget(1)->hide();
    }
    else {
        ui->participantSplitter->widget(1)->show();
    }
}

void Document::setChatHidden(bool b)
{
    // Hide/show the widget (contains the chat widget) below the code text edit
    if (b) {
        ui->codeChatSplitter->widget(1)->hide();
    }
    else {
        editor->setFocus();
        ui->codeChatSplitter->widget(1)->show();
    }
}

void Document::shiftLeft()
{
    editor->shiftLeft();
}

void Document::shiftRight()
{
    editor->shiftRight();
}

void Document::unCommentSelection()
{
    editor->unCommentSelection();
}

void Document::setHighlighter(int Highlighter)
{
    if (Highlighter == None) {
        highlighter->deleteLater();
    }
    else if (Highlighter == CPlusPlus) {

    }
    else if (Highlighter == Python) {

    }
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
    return ui->participantSplitter->widget(1)->isHidden();
}

void Document::findAll(QString searchString, bool ignoreCase)
{
    editor->findAll(searchString, ignoreCase);
}

void Document::findNext(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!editor->findNext(searchString, sensitivity, wrapAround, mode)) {
        emit notFound();
    }
}

void Document::findPrev(QString searchString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!editor->findPrev(searchString, sensitivity, wrapAround, mode)) {
        emit notFound();
    }
}

void Document::replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!editor->replaceAll(searchString, replaceString, sensitivity, wrapAround, mode)) {
        emit notFound();
    }
}

void Document::replace(QString replaceString)
{
    if (!editor->replace(replaceString)) {
        emit notFound();
    }
}

void Document::findReplace(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    if (!editor->findReplace(searchString, replaceString, sensitivity, wrapAround, mode)) {
        emit notFound();
    }
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

void Document::splitEditor()
{
    ui->editorSplitter->setOrientation(Qt::Vertical);
    bottomEditor->show();
}

void Document::splitEditorSideBySide()
{
    ui->editorSplitter->setOrientation(Qt::Horizontal);
    bottomEditor->show();
}

void Document::unSplitEditor()
{
    if (!isEditorSplit()) {
        return;
    }
    bottomEditor->hide();
}

bool Document::isEditorSplit()
{
    return !bottomEditor->isHidden();
}

bool Document::isEditorSplitSideBySide()
{
    return ui->editorSplitter->orientation() == Qt::Horizontal;
}

bool Document::isAnnounced()
{
    return isAlreadyAnnounced;
}






