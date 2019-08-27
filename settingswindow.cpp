#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "addprofiledialog.h"
#include "QDebug"
#include "string"
#include "QDesktopServices"
#include "QDir"
#include "QFileDialog"
#include "errorlogwaseraseddialog.h"
#include "mainwindow.h"

SettingsWindow::SettingsWindow(timeThread *tThread, std::vector<Profile> *profiles, NikkiDebug *nikkiDebug, Settings *settings, QDir* configDir, const float versionNumber, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow),
    configDirPtr(configDir),
    settings(settings),
    profiles(profiles),
    nikkiDebugStream(nikkiDebug),
    versionNumber(versionNumber)
{
    ui->setupUi(this);

    connect(tThread, SIGNAL(updateUI(const std::tm*)),
            this, SLOT(timeUpdate(const std::tm*)));

    initializeSettings();
    initializeProfiles();
    initializeHelp();
    initializeDebugPrompt();
    setVersionLabel();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

//--[ Buttons ]----------------------------------------------------------------

void SettingsWindow::on_buttonBox_accepted()
{
    *settings = tempSettings;
    *profiles = tempProfiles;
    this->close();
}

void SettingsWindow::on_buttonBox_rejected()
{
    this->close();
}

//--[ Profiles  ]--------------------------------------------------------------

void SettingsWindow::on_addProfileButton_released()
{
    QString tempNewProfileName;
    bool tempMakeDefault;

    addProfileDialog ProfileDialog(&tempNewProfileName, &tempMakeDefault);
    ProfileDialog.setModal(false);
    ProfileDialog.exec();

    createNewProfile(tempNewProfileName, tempMakeDefault);

    reinitializeProfiles();
}

void SettingsWindow::on_removeProfileButton_released()
{
    //uint32_t because otherwise the unsignedness of i is changed.
    uint32_t to_delete_position = 0;

    for(uint32_t i = 0; i < tempProfiles.size(); i++)
    {
        if(ui->listWidget->currentItem()->text() == QString::fromStdString(tempProfiles.at(i).name))
        {
            to_delete_position = i;
        }
    }

    if(tempProfiles.size() <= 1 || tempProfiles.at(to_delete_position).isDefault)
    {
        return;
    }
    else
    {
        tempProfiles.erase(tempProfiles.begin() + to_delete_position);
    }

    reinitializeProfiles();
}

void SettingsWindow::on_makeDefaultProfileButton_released()
{
    if(ui->listWidget->currentItem() != nullptr)
    {
        for(unsigned long long i = 0; i < tempProfiles.size(); i++)
        {
            if(ui->listWidget->selectedItems().first()->text() == QString::fromStdString(tempProfiles[i].name))
            {
                tempProfiles[i].isDefault = true;
            }
            else
            {
                tempProfiles[i].isDefault = false;
            }
        }
    }

    reinitializeProfiles();
}

//--[ Debug ]------------------------------------------------------------------

void SettingsWindow::on_openErrLogFileButton_released()
{
    std::stringstream temp;

    temp << "file:" << "///";
    temp << configDirPtr->path().toStdString();
    temp << "/nikkiErrLog.ne";

    //qDebug() << "temp:\t" << QString::fromStdString(temp.str());

    if(!QDesktopServices::openUrl(QUrl(QString::fromStdString(temp.str()), QUrl::TolerantMode))) // true when opened, false when not
    {
        ErrorlogWasErasedDialog ewed;
        ewed.setModal(true);
        ewed.exec();
    }

}

void SettingsWindow::on_openFileLocationButton_released()
{
    std::stringstream temp;

    temp << "file:" << "///";
    temp << configDirPtr->path().toStdString();

    //qDebug() << "temp:\t" << QString::fromStdString(temp.str());

    QDesktopServices::openUrl(QUrl(QString::fromStdString(temp.str()), QUrl::TolerantMode));
}

void SettingsWindow::on_eraseFileButton_released()
{
    bool reallyErase = false;

    ErrlogEraseDialog eled(&reallyErase);
    eled.setModal(true);
    eled.exec();

    if(reallyErase)
    {
        eraseErrlog();
    }
}

void SettingsWindow::eraseErrlog()
{
    std::stringstream tempErrlogPathStream;
    tempErrlogPathStream << configDirPtr->path().toStdString() << "/nikkiErrLog.ne";

    try
    {
        tempErrlogStream.open(tempErrlogPathStream.str(), std::ios::out);
    } catch (...) {
        // Debug message ...
    }

    if(tempErrlogStream.good() && tempErrlogStream.is_open())
    {
        tempErrlogStream.close();
        try
        {
            tempErrlogStream.open(tempErrlogPathStream.str(), std::ios::out | std::ios::trunc);
        } catch (...) {
            // Debug message ...
        }
    }

    NikkiDebugMessage("02", "1D", "ErrorLog was erased.");

    tempErrlogStream.close();
}

//--[ General Settings ]-------------------------------------------------------

void SettingsWindow::on_printAuthorCheckBox_stateChanged(int arg1)
{
    if(ui->printAuthorCheckBox->checkState() == Qt::CheckState::Checked)
    {
        tempSettings.printAuthorInEntry = true;
    }
    else if(ui->printAuthorCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempSettings.printAuthorInEntry = false;
    }

    reinitializeSettings();
}

void SettingsWindow::on_printTimeCheckBox_stateChanged(int arg1)
{
    if(ui->printTimeCheckBox->checkState() == Qt::CheckState::Checked)
    {
        tempSettings.printTimeInEntry = true;
    }
    else if(ui->printTimeCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempSettings.printTimeInEntry = false;
    }

    reinitializeSettings();
}

void SettingsWindow::on_printSeperationLineCheckBox_stateChanged(int arg1)
{

    if(ui->printSeperationLineCheckBox->checkState() == Qt::CheckState::Checked)
    {
        tempSettings.printLineInEntry = true;
    }
    else if(ui->printSeperationLineCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempSettings.printLineInEntry = false;
    }

    //*nikkiDebugStream << "printLineInEntry = " << tempSettings.printLineInEntry << "\n";

    reinitializeSettings();
}


void SettingsWindow::on_useDefaultCheckBox_stateChanged(int arg1)
{
    if(ui->useDefaultCheckBox->checkState() == Qt::CheckState::Checked)
    {
        tempSettings.useDefaultFilePath = true;
    }
    else if(ui->useDefaultCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempSettings.useDefaultFilePath = false;
    }

    //*nikkiDebugStream << "useDefaultFilePath = " << tempSettings.useDefaultFilePath << "\n";

    reinitializeSettings();
}

void SettingsWindow::on_nikkisPathBrowseButton_released()
{
    QString tempDirPath = QFileDialog::getExistingDirectory();

    tempSettings.defaultEntryPath = tempDirPath.toStdString();

    ui->nikkisPathLineEdit->setText(tempDirPath);
}


void SettingsWindow::on_closeCommandCheckBox_stateChanged(int arg1)
{
    if(ui->closeCommandCheckBox->checkState() == Qt::CheckState::Checked)
    {
        tempSettings.askBeforeClose = true;
    }
    else if(ui->closeCommandCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempSettings.askBeforeClose = false;
    }

    //*nikkiDebugStream << "askBeforeClose = " << tempSettings.askBeforeClose << "\n";

    reinitializeSettings();
}

void SettingsWindow::on_unicodeCheckBox_stateChanged(int arg1)
{
    if(ui->unicodeCheckBox->checkState() == Qt::CheckState::Checked)
    {
        tempSettings.useUnicodeEncoding = true;
    }
    else if(ui->unicodeCheckBox->checkState() == Qt::CheckState::Unchecked)
    {
        tempSettings.useUnicodeEncoding = false;
    }

    //*nikkiDebugStream << "useUnicodeEncoding = " << tempSettings.useUnicodeEncoding << "\n";

    reinitializeSettings();
}

//--[ Help ]-------------------------------------------------------------------

void SettingsWindow::initializeHelp()
{
    ui->helpComboBox->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);
}

void SettingsWindow::on_Sunburst275Button_released()
{
    QString link = "https://sunburst275.jimdofree.com/";
    QDesktopServices::openUrl(QUrl(link));
}

void SettingsWindow::on_helpComboBox_currentIndexChanged(const QString &arg1)
{
    if(ui->helpComboBox->currentText() == "General")
    {
        ui->stackedWidget->setCurrentIndex(0);
    }

    else if(ui->helpComboBox->currentText() == "First start")
    {
        ui->stackedWidget->setCurrentIndex(1);
    }

    else if(ui->helpComboBox->currentText() == "Profiles")
    {
        ui->stackedWidget->setCurrentIndex(2);
    }

    else if(ui->helpComboBox->currentText() == "Diary entries")
    {
        ui->stackedWidget->setCurrentIndex(3);
    }

    else if(ui->helpComboBox->currentText() == "Debug")
    {
        ui->stackedWidget->setCurrentIndex(4);
    }

}

void SettingsWindow::on_walkthroughButton_released()
{
    QString link = "https://sunburst275.jimdofree.com/software/simple-nikki/walkthrough/";
    QDesktopServices::openUrl(QUrl(link));
}

void SettingsWindow::on_feedbackButton_released()
{
    QString link = "https://sunburst275.jimdofree.com/software/simple-nikki/discussion/";
    QDesktopServices::openUrl(QUrl(link));
}

//--[ Methods ]----------------------------------------------------------------

void SettingsWindow::initializeSettings()
{
    tempSettings = *settings;

    reinitializeSettings();
}

void SettingsWindow::reinitializeSettings()
{
    // Default entry path
    if(tempSettings.useDefaultFilePath)
    {
        // Disable browse & lineEdit
        ui->nikkisPathBrowseButton->setDisabled(true);
        ui->nikkisPathLineEdit->setDisabled(true);
        // Tick use Default checkbox
        ui->useDefaultCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        // Enable browse & lineEdit
        ui->nikkisPathBrowseButton->setDisabled(false);
        ui->nikkisPathLineEdit->setDisabled(false);
        // Untick use Default checkbox
        ui->useDefaultCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }

    ui->nikkisPathLineEdit->setText(QString::fromStdString(tempSettings.defaultEntryPath));

    // Print Author in entry file
    if(tempSettings.printAuthorInEntry)
    {
        ui->printAuthorCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->printAuthorCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }

    // Print Time in entry file
    if(tempSettings.printTimeInEntry)
    {
        ui->printTimeCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->printTimeCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }

    // Print line in entry file
    if(!tempSettings.printAuthorInEntry && !tempSettings.printTimeInEntry)
    {
        ui->printSeperationLineCheckBox->setEnabled(false);
        tempSettings.printLineInEntry = false;
    }
    else
    {
        ui->printSeperationLineCheckBox->setEnabled(true);

        if(tempSettings.printLineInEntry)
        {
            ui->printSeperationLineCheckBox->setCheckState(Qt::CheckState::Checked);
        }
        else
        {
            ui->printSeperationLineCheckBox->setCheckState(Qt::CheckState::Unchecked);
        }
    }

    // Ask before close
    if(tempSettings.askBeforeClose)
    {
        ui->closeCommandCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->closeCommandCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }

    // Use UTF16- Encoding
    if(tempSettings.useUnicodeEncoding)
    {
        ui->unicodeCheckBox->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->unicodeCheckBox->setCheckState(Qt::CheckState::Unchecked);
    }
}

void SettingsWindow::initializeProfiles()
{
    tempProfiles = *profiles;

    reinitializeProfiles();
}

void SettingsWindow::reinitializeProfiles()
{
    ui->listWidget->clear();

    for(unsigned long long i = 0; i < tempProfiles.size(); i++)
    {
        // Add item
        ui->listWidget->addItem(QString::fromStdString(tempProfiles.at(i).name));

        // Mark as default in list
        if(tempProfiles.at(i).isDefault)
        {
            ui->listWidget->item(int(i))->setForeground(Qt::red);
        }
    }
}

void SettingsWindow::initializeDebugPrompt()
{
    if(!nikkiDebugStream->str().empty())
    {
        ui->debugTextEdit->setPlainText(QString::fromStdString(nikkiDebugStream->str()));
    }
    else
    {
        ui->debugTextEdit->setPlainText("Nothing to see here. Go on.");
    }
}

void SettingsWindow::createNewProfile(QString name, bool makeDefault)
{
    if(!name.isEmpty())
    {
        if(makeDefault)
        {
            for(unsigned long long i = 0; i < tempProfiles.size(); i++)
            {
                tempProfiles[i].isDefault = false;
            }
        }

        Profile newProfile = {name.toStdString(), makeDefault};

        tempProfiles.push_back(newProfile);

        MainWindow::contains_not_of_charSet(newProfile.name);

    }
}

void SettingsWindow::setVersionLabel()
{
    std::stringstream ss;
    ss.precision(2);
    ss << "Version:\t";
    ss << std::fixed << versionNumber;

    ui->versionLabel->setText(QString::fromStdString(ss.str()));
}

void SettingsWindow::timeUpdate(const std::tm* tempTime)
{
    this->tempTime = tempTime;

    std::stringstream cTime;
    cTime << std::setfill('0') << std::setw(3) << tempTime->tm_yday << "|";
    cTime << std::setfill('0') << std::setw(2) << tempTime->tm_hour << ":";
    cTime << std::setfill('0') << std::setw(2) << tempTime->tm_min << ":";
    cTime << std::setfill('0') << std::setw(2) << tempTime->tm_sec;
    tempCurrentTime = cTime.str();
}

void SettingsWindow::NikkiDebugMessage(std::string sourceFile, std::string callingFunction, std::string message)
{
    //nikkiDebugStream << currentTime << " [01|03]:\n";
    //228|16:32:53 [01|03]:

    std::stringstream messageStream;

    messageStream << tempCurrentTime << " [" << sourceFile << "|" << callingFunction << "]:\t" << message << "\n";

    *nikkiDebugStream << messageStream.str();

    initializeDebugPrompt();
}
