#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "document.h"
#include "connecttodocument.h"
#include "finddialog.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QMap<QWidget *, Document *> tabWidgetToDocumentMap;
    QString openPath;
    ConnectToDocument *connectDialog;
    FindDialog *findDialog;

    bool save(int index);
    bool maybeSave(int index);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    void readSettings();
    void writeSettings();

    QString myName; // global name used for connecting to documents

    QString getSystem();

    QStringList previousInfo;

private slots:
    void on_actionFile_New_triggered();
    void on_actionFile_Open_triggered();
    bool on_actionFile_Save_triggered();
    bool on_actionFile_Save_As_triggered();
    bool on_actionFile_Save_A_Copy_As_triggered();
    bool on_actionFile_Save_All_triggered();

    void on_actionFile_Close_triggered();
    void on_actionFile_Print_triggered();

    void on_actionEdit_Undo_triggered();
    void on_actionEdit_Redo_triggered();
    void on_actionEdit_Cut_triggered();
    void on_actionEdit_Copy_triggered();
    void on_actionEdit_Paste_triggered();
    void on_actionEdit_Find_triggered();

    void on_actionView_Line_Wrap_triggered();

    void on_actionTools_Announce_Document_triggered();
    void on_actionTools_Connect_to_Document_triggered();
    void on_actionTools_Preview_as_Html_triggered();

    void on_actionText_Shift_Left_triggered();
    void on_actionText_Shift_Right_triggered();
    void on_actionText_Comment_Line_triggered();

    void on_actionWindow_Hide_Show_Participants_triggered();
    void on_actionWindow_Hide_Show_Chat_triggered();
    void on_actionWindow_Split_triggered();

    void setUndoability(bool b);
    void setRedoability(bool b);

    void documentChanged(int index);
    void tabCloseClicked(int index);

    void findNextTriggered(QString str, bool ignoreCase, bool wrapAround);
    void findPrevTriggered(QString str, bool ignoreCase, bool wrapAround);

    void connectToDocument(QStringList *list);
    //void viewChanged();
};

#endif // MAINWINDOW_H
