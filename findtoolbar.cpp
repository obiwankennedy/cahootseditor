#include "findtoolbar.h"
#include "ui_findtoolbar.h"

FindToolBar::FindToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindToolBar)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
}

FindToolBar::~FindToolBar()
{
    delete ui;
}

void FindToolBar::returnPressed()
{
    emit findAll(ui->lineEdit->text());
}
