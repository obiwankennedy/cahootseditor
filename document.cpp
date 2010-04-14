#include "document.h"
#include "ui_document.h"

#include "utilities.h"
#include "enu.h"

#include <QDialog>
#include <QRegExp>
#include <QDebug>
#include <QFontMetrics>
#include <QWebView>
#include <QLayout>
#include <QMessageBox>
#include <QTextDocumentFragment>
#include <QDebug>

#include <srchiliteqt/Qt4SyntaxHighlighter.h>
#include <srchilite/versions.h>

using namespace srchiliteqt;

Document::Document(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Document)
{
    ui->setupUi(this);

    // Set up the editor
    delete ui->editorFrame;
    editor = new CodeEditor(this);
    QFontMetrics fm(editor->font());
    editor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(0, editor);
    
    // editor for split pane editing
    delete ui->bottomEditorFrame;
    bottomEditor = new CodeEditor(this);
    bottomEditor->setFont(editor->font());
    bottomEditor->setTabStopWidth(fm.averageCharWidth() * 4);
    ui->editorSplitter->insertWidget(1, bottomEditor);

    // we don't need the default document since we're using the document of the original editor
    bottomEditor->document()->deleteLater();
    bottomEditor->setDocument(editor->document());
    bottomEditor->hide();

    // editor highlighter
    //highlighter = new CppHighlighter(editor->document());
    srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
    highlighter->init("default.lang");


    // Participant frame
    delete ui->participantFrame;
    participantPane = new ParticipantsPane();
    ui->participantSplitter->insertWidget(1, participantPane);

    // Chat frame
    delete ui->chatFrame;
    chatPane = new ChatPane();
    ui->codeChatSplitter->insertWidget(1, chatPane);

    // Find all toolbar widget
    delete ui->findAllFrame;
    findAllToolbar = new FindToolBar(this);
    ui->editorVerticalLayout->insertWidget(1, findAllToolbar);
    findAllToolbar->hide();

    connect(findAllToolbar, SIGNAL(findAll(QString)), editor, SLOT(findAll(QString)));
    connect(findAllToolbar, SIGNAL(findNext(QString)), this, SLOT(findNext(QString)));
    connect(findAllToolbar, SIGNAL(findPrevious(QString)), this, SLOT(findPrevious(QString)));

    // Emit signals to the mainwindow when redoability/undoability changes
    connect(editor, SIGNAL(undoAvailable(bool)), this, SIGNAL(undoAvailable(bool)));
    connect(editor, SIGNAL(redoAvailable(bool)), this, SIGNAL(redoAvailable(bool)));

    QList<int> sizeList;
    sizeList << 9000 << 1;
    ui->codeChatSplitter->setSizes(sizeList);
    ui->participantSplitter->setSizes(sizeList);

    // Hide the panels that only matter if we're using the collaborative portion of the app
    setChatHidden(true);
    setParticipantsHidden(true);

    myName = "Owner"; // temporary

    startedCollaborating = false;
}

Document::~Document()
{    
    delete ui;
}

void Document::announceDocument(bool broadcastDocument)
{
    startedCollaborating = true;
    setChatHidden(false);
    setParticipantsHidden(false);

    server = new Server(editor, participantPane, chatPane, this);

    if (broadcastDocument) {
        server->startBroadcasting();
    }

    server->listen(QHostAddress::Any, 0); // Port is chosen automatically, listening on all NICs
    QString port = QString::number(server->serverPort(), 10);


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
    chatPane->appendChatMessage("Listening at " + ipAddress + ":" + port);

    participantPane->setConnectInfo(ipAddress, port);
}

void Document::connectToDocument(QStringList list)
{
    myName = list.at(0);    
    QString address = list.at(1);
    QString portString = list.at(2);

    int port = portString.toInt();
    participantPane->setOwnership(false);
    startedCollaborating = true;

    setChatHidden(false);
    setParticipantsHidden(false);
    editor->setReadOnly(true);
    participantPane->setDisabled(true);

    client = new Client(editor, participantPane, chatPane, this);
    client->setUsername(myName);
    client->connectToHost(QHostAddress(address), port);
    participantPane->setConnectInfo(address, portString);
}

void Document::setEditorFont(QFont font)
{
    editor->setFont(font);
    QFontMetrics fm(editor->font());
    editor->setTabStopWidth(fm.averageCharWidth() * 4);
}

void Document::setChatFont(QFont font)
{
    chatPane->setFont(font);
}

void Document::setParticipantsFont(QFont font)
{
    participantPane->setFont(font);
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
        editor->resize(QSize(9000, 9000));
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

void Document::resynchronizeTriggered()
{
    if (server) {
        server->resynchronize();
    }
    else if (client) {
        client->resynchronize();
    }
}

void Document::setHighlighter(int Highlighter)
{
    if (Highlighter == none) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("nohilite.lang");
    }
    else if (Highlighter == ada) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("ada.lang");
    }
    else if (Highlighter == c) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("c.lang");
    }
    else if (Highlighter == cplusplus) {
        // highlighter = new CppHighlighter(editor->document());
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("cpp.lang");
    }
    else if (Highlighter == csharp) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("csharp.lang");
    }
    else if (Highlighter == css) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("css.lang");
    }
    else if (Highlighter == flex) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("flex.lang");
    }
    else if (Highlighter == fortran) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("fortran.lang");
    }
    else if (Highlighter == glsl) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("glsl.lang");
    }
    else if (Highlighter == haxe) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("haxe.lang");
    }
    else if (Highlighter == html) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("html.lang");
    }
    else if (Highlighter == java) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("java.lang");
    }
    else if (Highlighter == javascript) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("javascript.lang");
    }
    else if (Highlighter == latex) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("latex.lang");
    }
    else if (Highlighter == ldap) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("ldap.lang");
    }
    else if (Highlighter == logtalk) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("logtalk.lang");
    }
    else if (Highlighter == lsm) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("lsm.lang");
    }
    else if (Highlighter == lua) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("lua.lang");
    }
    else if (Highlighter == m4) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("m4.lang");
    }
    else if (Highlighter == oz) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("oz.lang");
    }
    else if (Highlighter == pascal) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("pascal.lang");
    }
    else if (Highlighter == perl) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("perl.lang");
    }
    else if (Highlighter == php) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("php.lang");
    }
    else if (Highlighter == postscript) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("postscript.lang");
    }
    else if (Highlighter == prolog) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("prolog.lang");
    }
    else if (Highlighter == python) {
        // highlighter = new PythonHighlighter(editor->document());
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("python.lang");
    }
    else if (Highlighter == ruby) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("ruby.lang");
    }
    else if (Highlighter == scala) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("scala.lang");
    }
    else if (Highlighter == sml) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("sml.lang");
    }
    else if (Highlighter == sql) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("sql.lang");
    }
    else if (Highlighter == xml) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("xml.lang");
    }
    else if (Highlighter == xorg) {
        srchiliteqt::Qt4SyntaxHighlighter *highlighter = new Qt4SyntaxHighlighter(editor->document());
        highlighter->init("xorg.lang");
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

void Document::findAll()
{
    findAllToolbar->show();
    findAllToolbar->giveFocus();
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

void Document::replaceAll(QString searchString, QString replaceString, Qt::CaseSensitivity sensitivity, Enu::FindMode mode)
{
    if (!editor->replaceAll(searchString, replaceString, sensitivity, mode)) {
        QMessageBox::information(editor, tr("Cahoots"), tr("The string was not found."));
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
        bottomEditor->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    }
    else {
        editor->setLineWrapMode(QPlainTextEdit::NoWrap);
        bottomEditor->setLineWrapMode(QPlainTextEdit::NoWrap);
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
    QList<int> sizes;
    sizes << 500 << 500;
    ui->editorSplitter->setSizes(sizes);
}

void Document::splitEditorSideBySide()
{
    ui->editorSplitter->setOrientation(Qt::Horizontal);
    bottomEditor->show();
    QList<int> sizes;
    sizes << 500 << 500;
    ui->editorSplitter->setSizes(sizes);
}

bool Document::docHasCollaborated()
{
    return startedCollaborating;
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

void Document::findNext(QString string)
{
    editor->findNext(string, Qt::CaseInsensitive, true, Enu::Contains);
}

void Document::findPrevious(QString string)
{
    editor->findPrev(string, Qt::CaseInsensitive, true, Enu::Contains);
}

void Document::setOwnerName(QString name)
{
    server->setOwnerName(name);
}
