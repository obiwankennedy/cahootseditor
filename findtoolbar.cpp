#include "findtoolbar.h"
#include "ui_findtoolbar.h"

FindToolBar::FindToolBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindToolBar)
{
    ui->setupUi(this);

    connect(ui->lineEdit, SIGNAL(textEdited(QString)), this, SLOT(findTriggered(QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
}

FindToolBar::~FindToolBar()
{
    delete ui;
}

void FindToolBar::giveFocus()
{
    ui->lineEdit->setFocus();
    ui->lineEdit->setText("");
}

void FindToolBar::findTriggered(QString string)
{
    if (string.length() > 2) {
        emit findAll(string);
    }
}

void FindToolBar::findNextTriggered()
{
    emit findNext(ui->lineEdit->text());
}

void FindToolBar::closeButtonClicked()
{
    this->hide();
}

void FindToolBar::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        closeButtonClicked();
    }
    else if (ui->lineEdit->hasFocus() && e->key() == Qt::Key_Return && e->modifiers() != Qt::ShiftModifier) {
        emit findNext(ui->lineEdit->text());
    }
    else if (ui->lineEdit->hasFocus() && e->key() == Qt::Key_Return && e->modifiers() == Qt::ShiftModifier) {
        emit findPrevious(ui->lineEdit->text());
    }
}
