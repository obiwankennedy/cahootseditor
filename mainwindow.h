#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "document.h"
#include "connecttodocument.h"
#include "finddialog.h"
#include "findtoolbar.h"
#include "announcedocumentdialog.h"
#include "aboutdialog.h"
#include "firstrundialog.h"
#include "preferencesdialog.h"

#include <QSettings>

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

private:
    Ui::MainWindow *ui;
    QMap<QWidget *, Document *> tabWidgetToDocumentMap;
    QString openPath;
    ConnectToDocument *connectDialog;
    AnnounceDocumentDialog *announceDocumentDialog;
    FindDialog *findDialog;
    PreferencesDialog *preferencesDialog;
    AboutDialog *aboutDialog;
    FirstRunDialog *firstRunDialog;

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

protected:
    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *, QEvent *event);

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
    void on_actionEdit_Find_All_triggered();
    void on_actionEdit_Find_triggered();

    void on_actionEdit_None_triggered();
    void on_actionEdit_C_triggered();
    void on_actionEdit_Python_triggered();

    void on_actionView_Line_Wrap_triggered();
    void on_actionView_Hide_Show_Participants_triggered();
    void on_actionView_Hide_Show_Chat_triggered();

    void on_actionTools_Announce_Document_triggered();
    void on_actionTools_Connect_to_Document_triggered();
    void on_actionTools_Preview_as_Html_triggered();
    void on_actionTools_Resynchronize_Document_triggered();
    void on_actionTools_Preferences_triggered();

    void on_actionText_Shift_Left_triggered();
    void on_actionText_Shift_Right_triggered();
    void on_actionText_Comment_Line_triggered();

    void on_actionWindow_Split_triggered();
    void on_actionWindow_Split_Side_by_Side_triggered();
    void on_actionWindow_Remove_Split_triggered();
    void on_actionWindow_Next_Document_triggered();
    void on_actionWindow_Previous_Document_triggered();

    void on_actionHelp_About_Cahoots_triggered();
    void on_actionHelp_About_Qt_triggered();

    void setUndoability(bool b);
    void setRedoability(bool b);

    void documentChanged(int index);
    void tabCloseClicked(int index);

    void findNextTriggered(QString str, Qt::CaseSensitivity, bool wrapAround, Enu::FindMode mode);
    void findPrevTriggered(QString str, Qt::CaseSensitivity, bool wrapAround, Enu::FindMode mode);
    void replaceAllTriggered(QString find, QString replace, Qt::CaseSensitivity sensitivity, Enu::FindMode mode);
    void replaceTriggered(QString replace);
    void findReplaceTriggered(QString find, QString replace, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode);

    void connectToDocument(QStringList list);
    void announceDocument(QString ownerName, Qt::CheckState broadcastCheckState, Qt::CheckState alwaysUserNameCheckState);

    void codeEditorChangeFont(QString fontString);
    void chatPaneChangeFont(QString fontString);
    void participantsChangeFont(QString fontString);
};

#endif // MAINWINDOW_H
