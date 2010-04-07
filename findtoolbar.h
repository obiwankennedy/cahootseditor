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

    void giveFocus();

private:
    Ui::FindToolBar *ui;

private slots:
    void returnPressed();

    void closeButtonClicked();

signals:
    void findAll(QString);
};

#endif // FINDTOOLBAR_H
