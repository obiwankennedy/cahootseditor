#include "firstrundialog.h"
#include "ui_firstrundialog.h"

#include "utilities.h"

FirstRunDialog::FirstRunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FirstRunDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
    if (Utilities::getSystem() == "Mac") {
        ui->labelPc->hide();
    }
    else {
        ui->labelMac->hide();
    }
}

FirstRunDialog::~FirstRunDialog()
{
    delete ui;
}

void FirstRunDialog::dialogAccepted()
{
    hide();
}
