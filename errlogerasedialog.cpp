#include "errlogerasedialog.h"
#include "ui_errlogerasedialog.h"

#include <QPushButton>

ErrlogEraseDialog::ErrlogEraseDialog(bool *reallyErase, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrlogEraseDialog)
{
    ui->setupUi(this);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Erase");
    //ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    reallyErasePtr = reallyErase;
}

ErrlogEraseDialog::~ErrlogEraseDialog()
{
    delete ui;
}

void ErrlogEraseDialog::on_buttonBox_accepted()
{
    *reallyErasePtr = true;
    this->close();
}

void ErrlogEraseDialog::on_buttonBox_rejected()
{
    *reallyErasePtr = false;
    this->close();
}
