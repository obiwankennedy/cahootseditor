#include "findtoolbar.h"
#include "ui_findtoolbar.h"

FindToolBar::FindToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindToolBar)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SIGNAL(findNext(QString)));
}

FindToolBar::~FindToolBar()
{
    delete ui;
}
