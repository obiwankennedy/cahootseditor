#include "firstrundialog.h"
#include "ui_firstrundialog.h"

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::dialogAccepted()
{
    hide();
}
