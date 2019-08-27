#include "closeoperationdialog.h"
#include "ui_closeoperationdialog.h"
#include "QPushButton"

CloseOperationDialog::CloseOperationDialog(Settings *tSettings, int *closeMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CloseOperationDialog),
    tempSettings(tSettings)
{
    ui->setupUi(this);

    closeModePtr = closeMode;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Keep entry");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("Discard");

    initializeUI();
}

CloseOperationDialog::~CloseOperationDialog()
{
    saveSettings();
    delete ui;
}

void CloseOperationDialog::initializeUI()
{
    if(tempSettings->askBeforeClose)
    {
        ui->checkBox->setCheckState(Qt::CheckState::Checked);
    }
    else if(!tempSettings->askBeforeClose)
    {
        ui->checkBox->setCheckState(Qt::CheckState::Unchecked);
    }
}

void CloseOperationDialog::saveSettings()
{
    if(ui->checkBox->checkState() == Qt::CheckState::Checked)
    {
        tempSettings->askBeforeClose = true;
    }
    else if(ui->checkBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempSettings->askBeforeClose = false;
    }
}

void CloseOperationDialog::on_buttonBox_accepted()
{
    internCloseMode = MainCloseMode::KeepEntry;
    this->close();
}

void CloseOperationDialog::on_buttonBox_rejected()
{
    internCloseMode = MainCloseMode::Discard;
    this->close();
}

void CloseOperationDialog::closeEvent(QCloseEvent *)
{
    *closeModePtr = internCloseMode;
    QApplication::quit();
}
