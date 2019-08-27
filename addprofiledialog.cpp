#include "addprofiledialog.h"
#include "ui_addprofiledialog.h"

addProfileDialog::addProfileDialog(QString* tempNewProfileName, bool* makeDefault, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addProfileDialog)
{
    ui->setupUi(this);
    profileNamePtr = tempNewProfileName;
    makeDefaultPtr = makeDefault;
}

addProfileDialog::~addProfileDialog()
{
    delete ui;
}

void addProfileDialog::on_buttonBox_accepted()
{
    if(ui->MakeDefaultCheckBox->checkState() == Qt::CheckState::Checked)
    {
        *makeDefaultPtr = true;
    }
    else
    {
        *makeDefaultPtr = false;
    }

    *profileNamePtr = ui->lineEdit->text();
    this->close();
}

void addProfileDialog::on_buttonBox_rejected()
{
    this->close();
}
