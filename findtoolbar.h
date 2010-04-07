#ifndef FINDTOOLBAR_H
#define FINDTOOLBAR_H

#include <QWidget>
#include <QKeyEvent>

#include "codeeditor.h"

namespace Ui {
    class FindToolBar;
}

class FindToolBar : public QWidget {
    Q_OBJECT
public:
    FindToolBar(QWidget *parent = 0);
    ~FindToolBar();

    void giveFocus();

private:
    Ui::FindToolBar *ui;
    CodeEditor *codeEditor;

private slots:
    void findTriggered(QString string);

    void findNextTriggered();

    void closeButtonClicked();

protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void findAll(QString);
    void findNext(QString);
};

#endif // FINDTOOLBAR_H
