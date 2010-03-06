#ifndef FINDTOOLBAR_H
#define FINDTOOLBAR_H

#include <QWidget>

namespace Ui {
    class FindToolBar;
}

class FindToolBar : public QWidget {
    Q_OBJECT
public:
    FindToolBar(QWidget *parent = 0);
    ~FindToolBar();

private:
    Ui::FindToolBar *ui;

signals:
    void findNext(QString);
};

#endif // FINDTOOLBAR_H
