#include "settingswereeraseddialog.h"
#include "ui_settingswereeraseddialog.h"

SettingsWereErasedDialog::SettingsWereErasedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWereErasedDialog)
{
    ui->setupUi(this);
}

SettingsWereErasedDialog::~SettingsWereErasedDialog()
{
    delete ui;
}

void SettingsWereErasedDialog::on_buttonBox_accepted()
{
    this->close();
}
