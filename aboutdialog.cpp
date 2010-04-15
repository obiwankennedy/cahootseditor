#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "utilities.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    if (Utilities::getSystem() == "Mac") {
        ui->labelPc->hide();
    }
    else {
        ui->labelMac->hide();
    }

}

AboutDialog::~AboutDialog()
{
    delete ui;
}
