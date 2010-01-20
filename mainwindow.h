#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "document.h"

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

    bool save(int index);
    bool maybeSave(int index);
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    void readSettings();
    void writeSettings();

    QString getSystem();

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

    void on_actionWindow_Show_Participants_triggered();
    void on_actionWindow_Hide_Participants_triggered();
    void on_actionWindow_Show_Chat_triggered();
    void on_actionWindow_Hide_Chat_triggered();

    void setUndoability(bool b);
    void setRedoability(bool b);

    void documentChanged(int index);
    void tabCloseClicked(int index);
    //void viewChanged();
};

#endif // MAINWINDOW_H
