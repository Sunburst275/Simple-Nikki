#include "firststartdialog.h"
#include "ui_firststartdialog.h"
#include "QPushButton"
#include "QFileDialog"

firstStartDialog::firstStartDialog(bool *useDefaultPath, QString *customPath, std::vector<profile>* profiles, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::firstStartDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Accept");
    tProfiles = profiles;
    useDefaultPathPtr = useDefaultPath;
    customPathPtr = customPath;

    initializeUI();
}

firstStartDialog::~firstStartDialog()
{
    delete ui;
}

void firstStartDialog::on_buttonBox_accepted()
{
    createCustomProfile();

    tempCustomPath = ui->pathLineEdit->text();
    checkCheckBoxState();
    checkPathLineEditState();

    *useDefaultPathPtr = tempUseDefaultPath;
    *customPathPtr = tempCustomPath;
}

void firstStartDialog::createCustomProfile()
{
    tProfiles->clear();

    QString profileName = ui->lineEdit->text();

    if(!profileName.isEmpty())
    {
        //qDebug() << "[firstStartDialog][on_buttonBox_accepted()]\t" << "createCustomProfile() : " << profileName;

        profile newProfile = {profileName.toStdString(), true};

        tProfiles->push_back(newProfile);
    }
    else
    {
        createDefaultProfile();
    }
}

void firstStartDialog::createDefaultProfile()
{
    //qDebug() << "[firstStartDialog][on_buttonBox_accepted()]\t" << "createDefaultProfile()";

    tProfiles->clear();

    profile newProfile = {"Default", true};

    tProfiles->push_back(newProfile);

}

void firstStartDialog::closeEvent(QCloseEvent*)
{
    createDefaultProfile();
    this->close();
}

void firstStartDialog::on_pathBrowseButton_pressed()
{
    QString tempDirPath = QFileDialog::getExistingDirectory();

    tempCustomPath = tempDirPath;

    ui->pathLineEdit->setText(tempCustomPath);
}

void firstStartDialog::on_pathCheckBox_stateChanged(int arg1)
{
    checkCheckBoxState();
}

void firstStartDialog::checkCheckBoxState()
{
    if(ui->pathCheckBox->checkState() == Qt::CheckState::Checked)
    {
        tempUseDefaultPath = true;

        ui->pathLineEdit->setEnabled(false);
        ui->pathBrowseButton->setEnabled(false);
    }

    if(ui->pathCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempUseDefaultPath = false;

        ui->pathLineEdit->setEnabled(true);
        ui->pathBrowseButton->setEnabled(true);
    }
}

void firstStartDialog::checkPathLineEditState()
{
    if(ui->pathLineEdit->text().toStdString().empty())
    {
        tempUseDefaultPath = true;
    }
}

void firstStartDialog::initializeUI()
{
    ui->pathCheckBox->setCheckState(Qt::CheckState::Checked);
    ui->pathBrowseButton->setEnabled(false);
    ui->pathLineEdit->setEnabled(false);
}
