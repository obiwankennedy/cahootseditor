#include "findtoolbar.h"
#include "ui_findtoolbar.h"

FindToolBar::FindToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindToolBar)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
}

FindToolBar::~FindToolBar()
{
    delete ui;
}

void FindToolBar::giveFocus()
{
    ui->lineEdit->setFocus();
}

void FindToolBar::returnPressed()
{
    emit findAll(ui->lineEdit->text());
}

void FindToolBar::closeButtonClicked()
{
    this->hide();
}
