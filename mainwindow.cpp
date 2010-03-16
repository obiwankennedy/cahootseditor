#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QFontDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>
#include <QCloseEvent>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(documentChanged(int)));
    connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseClicked(int)));

    connectDialog = new ConnectToDocument(this);
    connect(connectDialog, SIGNAL(connectToDocumentClicked(QStringList*)), this, SLOT(connectToDocument(QStringList*)));

    findDialog = new FindDialog(this);
    connect(findDialog, SIGNAL(findDialogFindNext(QString,bool,bool)), this, SLOT(findNextTriggered(QString,bool,bool)));
    connect(findDialog, SIGNAL(findDialogFindPrev(QString,bool,bool)), this, SLOT(findPrevTriggered(QString,bool,bool)));

    Document *document = new Document(ui->tab);
    QGridLayout *tabLayout = new QGridLayout;
    tabLayout->addWidget(document);
    tabLayout->setContentsMargins(0,0,0,0);
    ui->tabWidget->widget(0)->setLayout(tabLayout);

    findToolbar = new FindToolBar(this);
    ui->mainToolBar->addWidget(findToolbar);
    connect(findToolbar, SIGNAL(findAll(QString)), this, SLOT(findAllTriggered(QString)));

    tabWidgetToDocumentMap.insert(ui->tabWidget->currentWidget(), document);

    connect(document, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoability(bool)));
    connect(document, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoability(bool)));

    readSettings();
    openPath = QDir::homePath();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Protected closeEvent
void MainWindow::closeEvent(QCloseEvent *event)
{
    bool okToQuit = true;
    for (int i = 0; i < ui->tabWidget->count() && okToQuit; i++) {
        okToQuit = maybeSave(i);
    }
    if (okToQuit) {
        writeSettings();
        event->accept();
    }
    else {
        event->ignore();
    }
}

void MainWindow::readSettings()
{
    QSettings settings("Cahoots", "MainWindow");
    QDesktopWidget *desktop = QApplication::desktop();
    int width = static_cast<int>(desktop->width() * 0.80);
    int height = static_cast<int>(desktop->height() * 0.70);
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    QPoint pos = settings.value("pos", QPoint((screenWidth - width) / 2, (screenHeight - height) / 2)).toPoint();
    QSize size = settings.value("size", QSize(width, height)).toSize();
    resize(size);
    move(pos);

    myName = settings.value("name", "Owner").toString();
}

void MainWindow::writeSettings()
{
    QSettings settings("Cahoots", "MainWindow");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("name", myName);
}

QString MainWindow::getSystem() {
    #ifdef Q_WS_X11
    return QString("Linux");
    #endif

    #ifdef Q_WS_MAC
    return QString("Mac");
    #endif

    #ifdef Q_WS_QWS
    return QString("Embedded Linux");
    #endif

    #ifdef Q_WS_WIN
    return QString("Windows");
    #endif
}

// Save methods
bool MainWindow::save(int index)
{
    if (tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->curFile.isEmpty()) {
        return on_actionFile_Save_As_triggered();
    }
    else {
        return saveFile(tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->curFile);
    }
}

bool MainWindow::maybeSave(int index)
{
    if (tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->isModified()) {
        ui->tabWidget->setCurrentIndex(index);
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, "Cahoots",
                                   "The document has been modified.\n"
                                   "Do you want to save your changes?",
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save(index);
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO 8859-1"));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->getPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage("File saved", 4000);
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
     QFile file(fileName);
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("Application"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return;
     }

     QTextStream in(&file);
     in.setCodec(QTextCodec::codecForName("ISO 8859-1"));
     QApplication::setOverrideCursor(Qt::WaitCursor);
     tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->setPlainText(in.readAll());
     QApplication::restoreOverrideCursor();

     setCurrentFile(fileName);
     statusBar()->showMessage("File loaded", 4000);
}

void MainWindow::setCurrentFile(const QString &fileName)
{
     tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile = fileName;
     tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->setModified(false);
//     setWindowModified(false); // Possible future mechanic for showing in the title bar if the document isModified

     QString shownName;
     if (tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile.isEmpty())
         shownName = "untitled.txt";
     else
         shownName = strippedName(tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile);

     ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
     return QFileInfo(fullFileName).fileName();
}

// File menu items
void MainWindow::on_actionFile_New_triggered()
{
    int index = ui->tabWidget->addTab(new QWidget(), "untitled.txt");
    
    Document *document = new Document(ui->tabWidget->widget(index));
    QGridLayout *tabLayout = new QGridLayout;
    tabLayout->addWidget(document);
    tabLayout->setContentsMargins(0,0,0,0);
    ui->tabWidget->widget(index)->setLayout(tabLayout);

    tabWidgetToDocumentMap.insert(ui->tabWidget->widget(index), document);

    connect(document, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoability(bool)));
    connect(document, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoability(bool)));
    
    ui->tabWidget->setCurrentIndex(index);
    ui->actionTools_Announce_Document->setEnabled(true);


    ui->actionWindow_Next_Document->setEnabled(ui->tabWidget->count() > 1);
    ui->actionWindow_Previous_Document->setEnabled(ui->tabWidget->count() > 1);

}

void MainWindow::on_actionFile_Open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open text file",
            openPath,
            "Text files (*.cpp *.txt *.h)");
    if (!fileName.isEmpty()) {
        loadFile(fileName);

        int index = ui->tabWidget->addTab(new QWidget(), QFileInfo(fileName).fileName());

        Document *document = new Document(ui->tabWidget->widget(index));
        QGridLayout *tabLayout = new QGridLayout;
        tabLayout->addWidget(document);
        tabLayout->setContentsMargins(0,0,0,0);
        ui->tabWidget->widget(index)->setLayout(tabLayout);

        tabWidgetToDocumentMap.insert(ui->tabWidget->widget(index), document);

        connect(document, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoability(bool)));
        connect(document, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoability(bool)));

        ui->tabWidget->setCurrentIndex(index);

        openPath = QFileInfo(fileName).path();

        ui->actionTools_Announce_Document->setEnabled(true);

        ui->actionWindow_Next_Document->setEnabled(ui->tabWidget->count() > 1);
        ui->actionWindow_Previous_Document->setEnabled(ui->tabWidget->count() > 1);
    }
}

bool MainWindow::on_actionFile_Save_triggered()
{
    if (tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile.isEmpty()) {
        return on_actionFile_Save_As_triggered();
    }
    else {
        return saveFile(tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile);
    }
}

bool MainWindow::on_actionFile_Save_As_triggered()
{
    qApp->processEvents(); // Redraw so we see the document we're saving along with the dialog
    // This is in case program control changes documents on a "Save All" or closeEvent
    QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save As...",
            tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile.isEmpty() ?
                QDir::homePath() + "/untitled.txt" :
                tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile,
            "Text (*.txt)");
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MainWindow::on_actionFile_Save_A_Copy_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save A Copy As...",
            tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile.isEmpty() ?
                QDir::homePath() + "/untitled.txt" :
                tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->curFile,
            "Text (*.txt)");
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO 8859-1"));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->getPlainText();
    QApplication::restoreOverrideCursor();

    statusBar()->showMessage("File saved as a copy", 4000);
    return true;
}

bool MainWindow::on_actionFile_Save_All_triggered()
{
    bool isAllSaved = false;
    QWidget *originalWidget = ui->tabWidget->currentWidget();
    for (int i = 0; i < tabWidgetToDocumentMap.size(); i++)
    {
        ui->tabWidget->setCurrentIndex(i);
        isAllSaved += on_actionFile_Save_triggered();
    }
    ui->tabWidget->setCurrentWidget(originalWidget);
    return isAllSaved;
}

void MainWindow::on_actionFile_Close_triggered()
{
    tabCloseClicked(ui->tabWidget->currentIndex());

    ui->actionWindow_Next_Document->setEnabled(ui->tabWidget->count() > 1);
    ui->actionWindow_Previous_Document->setEnabled(ui->tabWidget->count() > 1);
}

void MainWindow::on_actionFile_Print_triggered()
{
    QTextDocument document(tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->getPlainText(), this);
    document.setDefaultFont(QFont("Courier", 10, -1));

    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle("Print");
    if (dialog->exec() == QDialog::Accepted) {
        document.print(&printer);
    }
}

// Edit menu items
void MainWindow::on_actionEdit_Undo_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->undo();
}

void MainWindow::on_actionEdit_Redo_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->redo();
}

void MainWindow::on_actionEdit_Cut_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->cut();
}

void MainWindow::on_actionEdit_Copy_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->copy();
}

void MainWindow::on_actionEdit_Paste_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->paste();
}

void MainWindow::on_actionEdit_Find_triggered()
{
    findDialog->show();
}

void MainWindow::on_actionEdit_None_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->setHighlighter(Document::None);
}

void MainWindow::on_actionEdit_C_triggered()
{

}

void MainWindow::on_actionEdit_Python_triggered()
{

}

void MainWindow::on_actionView_Line_Wrap_triggered()
{
    //Line Wrap
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->toggleLineWrap();
}

void MainWindow::on_actionTools_Announce_Document_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->announceDocument();
    ui->actionTools_Announce_Document->setEnabled(false);
}

void MainWindow::on_actionTools_Connect_to_Document_triggered()
{
    // Create our dialog and show it. When they user clicks "okay", we'll emit a signal to the mainwindow, and pass that to the document.
    connectDialog->show();
    connectDialog->setName(myName);
}

void MainWindow::on_actionTools_Preview_as_Html_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->previewAsHtml();
}

void MainWindow::on_actionText_Shift_Left_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->shiftLeft();
}

void MainWindow::on_actionText_Shift_Right_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->shiftRight();
}

void MainWindow::on_actionText_Comment_Line_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->unCommentSelection();
}

void MainWindow::on_actionWindow_Hide_Show_Participants_triggered()
{

    if (tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->isParticipantsHidden()) {
        tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->setParticipantsHidden(false);
    }
    else {
        tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->setParticipantsHidden(true);
    }
}

void MainWindow::on_actionWindow_Hide_Show_Chat_triggered()
{
    if (tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->isChatHidden()) {
        tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->setChatHidden(false);
    }
    else {
        tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->setChatHidden(true);
    }
}

void MainWindow::on_actionWindow_Split_triggered()
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->splitEditor();
    ui->actionWindow_Split->setDisabled(true);
    ui->actionWindow_Remove_Split->setEnabled(true);
}

void MainWindow::on_actionWindow_Remove_Split_triggered()
{
    ui->actionWindow_Split->setDisabled(false);
    ui->actionWindow_Remove_Split->setEnabled(false);
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->unSplitEditor();
}

void MainWindow::on_actionWindow_Next_Document_triggered()
{
    int numDocs = ui->tabWidget->count();
    int curTab = ui->tabWidget->currentIndex();
    if (numDocs == 1) {
        return;
    }
    ui->tabWidget->setCurrentIndex((curTab + 1) % numDocs);
}

void MainWindow::on_actionWindow_Previous_Document_triggered()
{
    int numDocs = ui->tabWidget->count();
    int curTab = ui->tabWidget->currentIndex();
    if (numDocs == 1) {
        return;
    }
    if (curTab == 0) {
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
    }
    else {
        ui->tabWidget->setCurrentIndex(curTab - 1);
    }
}

void MainWindow::setUndoability(bool b)
{
    ui->actionEdit_Undo->setEnabled(b);
}

void MainWindow::setRedoability(bool b)
{
    ui->actionEdit_Redo->setEnabled(b);
}

void MainWindow::documentChanged(int index)
{
    ui->actionEdit_Undo->setEnabled(tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->isUndoable());
    ui->actionEdit_Redo->setEnabled(tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->isRedoable());
    ui->actionWindow_Split->setDisabled(tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->isEditorSplit());
    ui->actionWindow_Remove_Split->setEnabled(tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->isEditorSplit());

    ui->actionTools_Announce_Document->setDisabled(tabWidgetToDocumentMap.value(ui->tabWidget->widget(index))->isAnnounced());
}

void MainWindow::tabCloseClicked(int index)
{
    if (maybeSave(index)) {
        if (ui->tabWidget->count() == 1) {
            on_actionFile_New_triggered();
        }
        ui->tabWidget->widget(index)->deleteLater();
        tabWidgetToDocumentMap.remove(ui->tabWidget->widget(index));
        ui->tabWidget->removeTab(index);
    }
}

void MainWindow::findAllTriggered(QString str)
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->findAll(str, true);
}

void MainWindow::findNextTriggered(QString str, bool ignoreCase, bool wrapAround)
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->findNext(str, ignoreCase, wrapAround);
}

void MainWindow::findPrevTriggered(QString str, bool ignoreCase, bool wrapAround)
{
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->findPrev(str, ignoreCase, wrapAround);
}

void MainWindow::connectToDocument(QStringList *list)
{
    on_actionFile_New_triggered();
    tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->connectToDocument(list);;
    if (tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->myName != "") {
        myName = tabWidgetToDocumentMap.value(ui->tabWidget->currentWidget())->myName;
    }
}
