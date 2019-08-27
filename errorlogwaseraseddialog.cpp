#include "errorlogwaseraseddialog.h"
#include "ui_errorlogwaseraseddialog.h"

ErrorlogWasErasedDialog::ErrorlogWasErasedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrorlogWasErasedDialog)
{
    ui->setupUi(this);
}

ErrorlogWasErasedDialog::~ErrorlogWasErasedDialog()
{
    delete ui;
}
