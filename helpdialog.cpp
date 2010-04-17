#include <QClipboard>
#include "helpdialog.h"
#include "ui_helpdialog.h"
#include "utilities.h"

HelpDialog::HelpDialog(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

//    connect(ui->treeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this,
//            SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

    ui->treeWidget->hide();

    if (Utilities::getSystem() == "Linux") {
        ui->treeWidget->setFont(QFont(Utilities::labelFont, 8));
    }
    else if (Utilities::getSystem() == "Windows") {
        ui->treeWidget->setFont(QFont(Utilities::labelFont, 8));
    }

    ui->webView->load(QUrl("qrc:/help/collaboration.html"));
}

HelpDialog::~HelpDialog()
{
    delete ui;
}

void HelpDialog::selectItem(QString string) {
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
        if ((*it)->text(0) == string) {
            (*it)->setSelected(true);
            ui->treeWidget->setCurrentItem((*it));
        } else {
            (*it)->setSelected(false);
        }
        ++it;
    }
}

void HelpDialog::onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*) {
    // Is this a subcategory?
    bool isHelpSubCat = ui->treeWidget->currentIndex().parent().isValid();
    // Parent row (if it has a parent, -1 else)
    int parentRow = ui->treeWidget->currentIndex().parent().row();
    // Row (if it has a parent, this is the child row)
    int row = ui->treeWidget->currentIndex().row();

//    qDebug() << "Selected: " << ui->treeWidget->currentIndex();
    // do nothing for now
}

// Public functions called by main window help menu items:

bool HelpDialog::hasFocus()
{
    return ui->webView->hasFocus();
}

void HelpDialog::copy()
{
    if (ui->webView->hasFocus()) {
        QApplication::clipboard()->setText(ui->webView->selectedText());
    }
}
