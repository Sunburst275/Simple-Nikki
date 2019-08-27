#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QDebug"
#include "QClipboard"
#include "QGuiApplication"

#include "settingswindow.h"
#include "firststartdialog.h"
#include "closeoperationdialog.h"
#include "settingswereeraseddialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tThread(),
    mainTime(nullptr),
    configDir(""),
    nikkiDir(""),
    profileDir(""),
    profiles({})
{
    ui->setupUi(this);

    //setWindowIcon(QIcon(":/simplenikkiicons"));

    // Timer
    connect(&tThread, SIGNAL(updateUI(const std::tm*)),
            this, SLOT(timeUpdate(const std::tm*)));
    tThread.setLooping(true);
    tThread.start();
    tThread.forceTimeUpdate();

    NikkiDebugMessage("01", "00", "Starting program.");

    if(tThread.isRunning())
    {
        NikkiDebugMessage("01", "00", "TimeThread is running.");
    }

    initializeProgram();
    initializeProfileBox();
}

MainWindow::~MainWindow()
{
    NikkiDebugMessage("01", "00", "Closing program.");
    saveErrorLog();
    //saveSettings();
    tThread.setLooping(false);
    tThread.exit();
    tThread.quit();
    tThread.terminate();
    delete ui;
}

//--[ Buttons ]----------------------------------------------------------------

void MainWindow::on_settingsButton_released()
{
    NikkiDebugMessage("01", "0F", "Settings opened.");

    // Hier einfügen ob "Save" oder "X" oder "Cancel" gedrückt wurde.

    chosenProfile = ui->profileBox->currentText();

    SettingsWindow settingsWindow(&tThread, &profiles, &nikkiDebugStream, &settings, &configDir, versionNumber, this);
    settingsWindow.setModal(true);
    settingsWindow.exec();

    NikkiDebugMessage("01", "0F", "Settings closed.");

    saveSettings();
    saveProfileData();
    reinitializeProfileBox();
}

void MainWindow::on_doneButton_released()
{
    NikkiDebugMessage("01", "10", "Done button released.");
    doneButtonAction();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    closeWindowAction();
}

// --[ Initialization ]--------------------------------------------------------

void MainWindow::initializeProfileBox()
{
    ui->profileBox->clear();

    for (unsigned long long i = 0; i < profiles.size(); i++)
    {
        if(profiles[i].isDefault)
            ui->profileBox->addItem(QString::fromStdString(profiles.at(i).name));
    }

    for (unsigned long long i = 0; i < profiles.size(); i++)
    {
        if(!profiles[i].isDefault)
            ui->profileBox->addItem(QString::fromStdString(profiles.at(i).name));
    }
}

void MainWindow::reinitializeProfileBox()
{
    bool stillExists = false;
    unsigned long long chosenIndex = 0;

    ui->profileBox->clear();

    if(!chosenProfile.toStdString().empty())
    {
        for (unsigned long long i = 0; i < profiles.size(); i++)
        {
            if(profiles[i].name == chosenProfile.toStdString())
            {
                stillExists = true;
                chosenIndex = i;
                ui->profileBox->addItem(QString::fromStdString(profiles.at(i).name));
            }
        }

        if(stillExists)
        {
            for (unsigned long long i = 0; i < profiles.size(); i++)
            {
                if(profiles[i].name != profiles[chosenIndex].name)
                    ui->profileBox->addItem(QString::fromStdString(profiles.at(i).name));
            }
        }
        else
        {
            initializeProfileBox();
        }
    }
    else
    {
        initializeProfileBox();
    }
}

void MainWindow::initializeProgram()
{
    // Checking for directories and initializing paths for them
    SettingsLoadState settingsLoadState = SettingsLoadState::FileOpenError;
    ProfilesLoadState profilesLoadState = ProfilesLoadState::FileOpenError;

    // Setting paths for program
    defaultPath = QDir::currentPath();

    configDirPath = defaultPath + configDirName;

    // Check if configDir exists
    configDir.setPath(configDirPath);
    if(configDir.exists())
    {
        // CHECK WHETHER SETTINGS/PROFILES EXIST
        NikkiDebugMessage("01", "01", "ConfigDir exists. Trying to load data...");
        settingsLoadState = loadSettings();
        profilesLoadState = loadProfileData();
    }
    else
    {
        NikkiDebugMessage("01", "01", "ConfigDir didnt exist. Creating new one.");
        configDir.mkdir(configDir.path());
        firstStartPrompt();                 // If configDir didnt exist, its the first startup
    }

    if(settingsLoadState != SettingsLoadState::Success && profilesLoadState == ProfilesLoadState::Success)
    {
        NikkiDebugMessage("01", "01", "Settings have not been loaded. Warning user.");
        // Show warning message to user

        SettingsWereErasedDialog swed;
        swed.setModal(true);
        swed.exec();
    }
}

void MainWindow::firstStartPrompt()
{
    NikkiDebugMessage("01", "0B", "Starting FirstStartPrompt");

    bool tempUseDefaultPath;
    QString tempCustomPath;

    firstStartDialog fsd(&tempUseDefaultPath, &tempCustomPath, &profiles);
    fsd.setModal(true);
    fsd.exec();

    NikkiDebugMessage("01", "0B", "Closing FirstStartPrompt");

    settings.useDefaultFilePath = tempUseDefaultPath;
    settings.defaultEntryPath = tempCustomPath.toStdString();
}

// --[ Saving and loading ]----------------------------------------------------

MainWindow::ProfilesSaveState MainWindow::saveProfileData()
{
    bool errorOccured = false;
    int errorIndex = 0;

    std::ofstream profileOutputStream;

    NikkiDebugMessage("01", "02", "Saving profiles...");

    // If configDir doesnt exist, build a new one.
    if(!configDir.exists())
    {
        configDir.mkdir(configDir.path());
    }

    if(profilesFilePath.toStdString().empty())
    {
        profilesFilePath = configDirPath + profilesFileName;
    }

    profileOutputStream.open(profilesFilePath.toStdString(), std::ios::out | std::ios::binary | std::ios::trunc);

    qDebug() << "profilesFilePath :\t" << profilesFilePath;

    if(profileOutputStream.good() && profileOutputStream.is_open())
    {

        // Write data
        for(unsigned long long i = 0; i < profiles.size(); i++)
        {
            // 1: Save string length independently. (tempLength)
            size_t tempLength = profiles[i].name.size();
            if(!errorOccured)
            {
                try
                {
                    profileOutputStream.write(reinterpret_cast<char*>(&tempLength), sizeof(tempLength));
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "02", "tempLength not savable.");
                }
            }

            // 2: Save string with string length
            if(!errorOccured)
            {
                try
                {
                    profileOutputStream.write(profiles[i].name.c_str(), tempLength);
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "02", "name not savable.");
                }
            }

            // 3: Save isDefault
            if(!errorOccured)
            {
                try
                {
                    profileOutputStream.write(reinterpret_cast<char*>(&profiles[i].isDefault), sizeof(profiles[i].isDefault));
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "02", "isDefault not savable.");
                }
            }
        }

        if(errorOccured)
        {
            NikkiDebugMessage("01", "02", "An error occured during the saving process (profiles).");

            std::stringstream temp;
            temp << "Error occured @ profile #(" << errorIndex << ").";
            NikkiDebugMessage("01", "03", temp.str());

            if(!profilesFilePath.toStdString().empty())
            {
                std::stringstream temp2;
                temp2 << "SettingsFilePath: " << profilesFilePath.toStdString();
                NikkiDebugMessage("01", "02", temp2.str());
            }

            profileOutputStream.close();
            return ProfilesSaveState::Failure;
        }
    }
    else
    {
        NikkiDebugMessage("01", "02", "Profiles file could not be opened/created.");

        if(!profilesFilePath.toStdString().empty())
        {
            std::stringstream temp;
            temp << "SettingsFilePath: " << profilesFilePath.toStdString();
            NikkiDebugMessage("01", "02", temp.str());
        }

        profileOutputStream.close();
        return ProfilesSaveState::FileOpenError;
    }

    NikkiDebugMessage("01", "02", "Profiles were successfully saved.");
    profileOutputStream.close();
    return ProfilesSaveState::Success;
}

MainWindow::ProfilesLoadState MainWindow::loadProfileData()
{
    bool errorOccured = false;

    std::ifstream profileInputStream;

    NikkiDebugMessage("01", "03", "Loading profiles...");

    profilesFilePath = configDirPath + profilesFileName;

    // Set up filestream for config
    profileInputStream.open(profilesFilePath.toStdString(), std::ios::in | std::ios::binary);

    if(profileInputStream.good() && profileInputStream.is_open())
    {
        int i = 0;

        // Load data
        while(!profileInputStream.eof())
        {
            Profile tempProfile = {};

            // 1: Fetch namelength
            size_t tempLength = 0;
            if(!errorOccured)
            {
                try
                {
                    profileInputStream.read(reinterpret_cast<char*>(&tempLength), sizeof(tempLength));
                    //qDebug() << "profiles[" << i << "] size: " << tempLength;
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "03", "tempLength not loadable.");
                }
            }

            // 2: Load name with length of name
            std::string tempName;
            if(!errorOccured)
            {
                try
                {
                    tempName.resize(tempLength);
                    profileInputStream.read(&tempName[0], tempLength * sizeof(char));
                    //qDebug() << "profiles[" << i << "] name: " << QString::fromStdString(tempName);
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "03", "tempName not loadable.");
                }
            }

            // 3: Load isDefault
            bool tempIsDefault = false;
            if(!errorOccured)
            {
                try
                {
                    profileInputStream.read(reinterpret_cast<char*>(&tempIsDefault), sizeof(bool));
                    //qDebug() << "profiles[" << i << "] isDefault: " << tempIsDefault;
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "03", "tempIsDefault not loadable.");
                }
            }

            // 4: Insert profile into program
            if(tempLength > 0  && !tempName.empty() && !errorOccured)
            {
                try
                {
                    tempProfile = {tempName, tempIsDefault};
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "03", "Problem during profile initiation occured.");
                }

                try
                {
                    profiles.push_back(tempProfile);
                } catch (...)
                {
                    errorOccured = true;
                    NikkiDebugMessage("01", "03", "Problem during profile pushback occured.");
                }
            }

            if(errorOccured)
            {
                std::stringstream temp;
                temp << "Error occured @ profile #(" << i << ").";

                NikkiDebugMessage("01", "03", temp.str());
                i++;
                errorOccured = false;
            }
        }

        if(errorOccured)
        {
            NikkiDebugMessage("01", "03", "A problem occured during the loading process (profiles).");

            std::stringstream temp;
            temp << "SettingsFilePath: " << profilesFilePath.toStdString();
            NikkiDebugMessage("01", "03", temp.str());

            profileInputStream.close();
            return ProfilesLoadState::Failure;
        }
    }
    else
    {
        NikkiDebugMessage("01", "03", "Profile file could not be opened. Program will be reinitiated...");

        firstStartPrompt();

        profileInputStream.close();
        return ProfilesLoadState::FileOpenError;
    }

    NikkiDebugMessage("01", "03", "Profiles were successfully loaded.");
    profileInputStream.close();
    return ProfilesLoadState::Success;
}


// New Stuff
MainWindow::EntryWriteState MainWindow::makeNikkiEntry()
{
    bool unknownProfile = false;

    NikkiDebugMessage("01", "04", "Create Nikki entry");

    // Fetch current user
    currentProfile = ui->profileBox->currentText();

    unknownProfile = contains_not_of_charSet(currentProfile.toStdString());

    if(unknownProfile)
    {
        currentProfile.clear();
        currentProfile = "Unknown";
    }

    if(settings.useDefaultFilePath || settings.defaultEntryPath.empty())
    {
        NikkiDebugMessage("01", "04", "-> using default path");
        nikkiDirPath = defaultPath + nikkiDirName;
    }
    else
    {
        NikkiDebugMessage("01", "04", "-> using custom path");
        nikkiDirPath = QString::fromStdString(settings.defaultEntryPath) + nikkiDirName;
    }

    nikkiDir.setPath(nikkiDirPath);
    if(!nikkiDir.exists())
    {
        nikkiDir.mkdir(nikkiDir.path());
        NikkiDebugMessage("01", "04", "NikkiDir didnt exist. Created new one.");
    }

    profileDirPath = nikkiDirPath + currentProfile;

    profileDir.setPath(profileDirPath);
    if(!profileDir.exists())
    {
        profileDir.mkdir(profileDir.path());
        NikkiDebugMessage("01", "04", "ProfileDir didnt exist. Created new one.");
    }

    NikkiDebugMessage("01", "04", "Now writing...");
    if(unknownProfile)
    {
        NikkiDebugMessage("01", "04", "-> UFT-16 encoded (Unknown profile)");
        writeEntry_UTF16(true);
    }
    else if(settings.useUnicodeEncoding)
    {
        // Write utf normal
        NikkiDebugMessage("01", "04", "-> UFT-16 encoded");
        return writeEntry_UTF16();
    }
    else
    {
        // Write normal
        NikkiDebugMessage("01", "04", "-> Plain");
        return writeEntry();
    }

    return EntryWriteState::Failure;


}

MainWindow::EntryWriteState MainWindow::writeEntry()
{
    bool errorOccured = false;

    QString writtenEntry = ui->editArea->toPlainText();
    std::string toWriteEntry = writtenEntry.toStdString();

    std::string filePath = profileDir.path().toStdString();

    // Make fileName
    // fileName format: profileName_YYYY-MM-DD_hh-mm-ss
    std::stringstream fileNameStream;
    fileNameStream << currentProfile.toStdString() << "_"
                   << std::setfill('0') << std::setw(4) << mainTime->tm_year + 1900  << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mon + 1  << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mday << "_"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_hour << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_min  << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_sec;
    // Make date inside file
    // date inside file format: YYYY.MM.DD, hh:mm:ss
    std::stringstream fileDateStream;
    fileDateStream << std::setfill('0') << std::setw(4) << mainTime->tm_year + 1900  << "."
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mon + 1  << "."
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mday << ", "
                   << std::setfill('0') << std::setw(2) << mainTime->tm_hour << ":"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_min  << ":"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_sec;

    // Make whole file with filepath
    filePath += "/" + fileNameStream.str() + ".txt";

    std::ofstream nikkiOutputStream;

    // Here: write data into file
    if(!toWriteEntry.empty())
    {
        nikkiOutputStream.open(filePath, std::ios_base::out);

        if(nikkiOutputStream.good() && nikkiOutputStream.is_open())
        {
            // Write file

            if(settings.printAuthorInEntry)
            {
                try
                {
                    nikkiOutputStream << "Author:\t" << currentProfile.toStdString() << "\n";
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            if(settings.printTimeInEntry)
            {
                try
                {
                    nikkiOutputStream << "Date:\t" << fileDateStream.str() << "\n";
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            if(settings.printLineInEntry)
            {
                try
                {
                    for(int i = 0; i < maxLineWidth; i++)
                    {
                        nikkiOutputStream << "_";
                    }
                    nikkiOutputStream << "\n";
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            if(settings.printAuthorInEntry || settings.printTimeInEntry)
            {
                try
                {
                    nikkiOutputStream << "\n";
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            try
            {
                nikkiOutputStream << toWriteEntry;
            } catch (...)
            {
                errorOccured = true;
            }

            if(errorOccured)
            {
                NikkiDebugMessage("01", "05", "A problem occured during writing the Nikki entry.");

                std::stringstream temp;
                temp << "SettingsFilePath: " << filePath;
                NikkiDebugMessage("01", "05", temp.str());

                nikkiOutputStream.close();
                return EntryWriteState::Failure;
            }
        }
        else
        {
            NikkiDebugMessage("01", "05", "Nikki file could not be opened.");

            std::stringstream temp;
            temp << "SettingsFilePath: " << filePath;
            NikkiDebugMessage("01", "05", temp.str());

            nikkiOutputStream.close();
            return EntryWriteState::FileOpenError;
        }
    }
    else
    {
        NikkiDebugMessage("01", "05", "Entry field was empty.");
    }

    if(!toWriteEntry.empty())
    {
        NikkiDebugMessage("01", "05", "Nikki was successfully created.");
    }
    nikkiOutputStream.close();
    return EntryWriteState::Success;
}

MainWindow::EntryWriteState MainWindow::writeEntry_UTF16(bool unknownProfile)
{
    bool errorOccured = false;

    std::locale loc(std::locale(), new std::codecvt_utf16<wchar_t>);

    QString writtenEntry = ui->editArea->toPlainText();
    u16string toWriteEntry = writtenEntry.toStdU16String();

    std::string filePath = profileDir.path().toStdString();

    // Make fileName
    // fileName format: profileName_YYYY-MM-DD_hh-mm-ss
    std::stringstream fileNameStream;
    fileNameStream << currentProfile.toStdString() << "_"
                   << std::setfill('0') << std::setw(4) << mainTime->tm_year + 1900  << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mon + 1  << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mday << "_"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_hour << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_min  << "-"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_sec;
    // Make date inside file
    // date inside file format: YYYY.MM.DD, hh:mm:ss
    std::stringstream fileDateStream;
    fileDateStream << std::setfill('0') << std::setw(4) << mainTime->tm_year + 1900  << "."
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mon + 1  << "."
                   << std::setfill('0') << std::setw(2) << mainTime->tm_mday << ", "
                   << std::setfill('0') << std::setw(2) << mainTime->tm_hour << ":"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_min  << ":"
                   << std::setfill('0') << std::setw(2) << mainTime->tm_sec;

    // Make whole file with filepath
    filePath += "/" + fileNameStream.str() + ".txt";

    std::basic_ofstream<char16_t> nikkiOutputStream;

    // Here: write data into file
    if(!toWriteEntry.empty())
    {
        nikkiOutputStream.open(filePath, std::ios::out);
        nikkiOutputStream.imbue(loc);
        nikkiOutputStream << char16_t(0xfeff); // BOM (actually: wchar_t(0xfeff), for UNICODE char16_t)

        if(nikkiOutputStream.good() && nikkiOutputStream.is_open())
        {
            // Write file
            if(settings.printAuthorInEntry || unknownProfile)
            {
                try
                {
                    nikkiOutputStream << utf8_to_utf16("Author:\t") << ui->profileBox->currentText().toStdU16String() << utf8_to_utf16("\n");
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            if(settings.printTimeInEntry)
            {
                try
                {
                    nikkiOutputStream << utf8_to_utf16("Date:\t") << utf8_to_utf16(fileDateStream.str()) << utf8_to_utf16("\n");
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            if(settings.printLineInEntry || unknownProfile)
            {
                try
                {
                    for(int i = 0; i < maxLineWidth; i++)
                    {
                        nikkiOutputStream << utf8_to_utf16("_");
                    }
                    nikkiOutputStream << utf8_to_utf16("\n");
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            if(settings.printAuthorInEntry || settings.printTimeInEntry || unknownProfile)
            {
                try
                {
                    nikkiOutputStream << utf8_to_utf16("\n");
                } catch (...)
                {
                    errorOccured = true;
                }
            }

            try
            {
                nikkiOutputStream << toWriteEntry;
            } catch (...)
            {
                errorOccured = true;
            }

            if(errorOccured)
            {
                NikkiDebugMessage("01", "0E", "A problem occured during writing the Nikki entry.");

                std::stringstream temp;
                temp << "SettingsFilePath: " << filePath;
                NikkiDebugMessage("01", "0E", temp.str());

                nikkiOutputStream.close();
                return EntryWriteState::Failure;
            }
        }
        else
        {
            NikkiDebugMessage("01", "0E", "Nikki file could not be opened.");

            std::stringstream temp;
            temp << "SettingsFilePath: " << filePath;
            NikkiDebugMessage("01", "0E", temp.str());

            nikkiOutputStream.close();
            return EntryWriteState::FileOpenError;
        }
    }
    else
    {
        NikkiDebugMessage("01", "0E", "Entry field was empty.");
    }

    if(!toWriteEntry.empty())
    {
        NikkiDebugMessage("01", "0E", "Nikki was successfully created.");
    }
    nikkiOutputStream.close();
    return EntryWriteState::Success;
}


void MainWindow::saveErrorLog()
{
    std::ofstream debugOutputStream;

    if(configDir.exists())
    {
        // write file into configDir
        errorLogFilePath = configDirPath + errorLogFileName;
    }
    else
    {
        errorLogFilePath = defaultPath + errorLogFileName;
    }

    //qDebug() << "errorLogFilePath: " << errorLogFilePath;

    debugOutputStream.open(errorLogFilePath.toStdString(), std::ios_base::out | std::ios_base::app);

    if(debugOutputStream.good() && debugOutputStream.is_open())
    {
        try {
            debugOutputStream << nikkiDebugStream.str();

            debugOutputStream << "\n";
            for(int i = 0; i < maxLineWidth/2; i++)
            {
                debugOutputStream << "-";
            }
            debugOutputStream << "\n";

            debugOutputStream << "pA\t=\t" << settings.printAuthorInEntry << "\n";      // Print Author
            debugOutputStream << "pD\t=\t" << settings.printTimeInEntry << "\n";        // Print Date
            debugOutputStream << "pL\t=\t" << settings.printLineInEntry << "\n";        // Print Line
            debugOutputStream << "Abc\t=\t" << settings.askBeforeClose << "\n";         // Ask before close
            debugOutputStream << "16\t=\t" << settings.useUnicodeEncoding << "\n";      // UTF16- Encoding
            debugOutputStream << "def\t=\t" << settings.useDefaultFilePath << "\n";     // Use default file path
            debugOutputStream << "#p\t=\t" << profiles.size() << "\n";                  // Number of profiles

            debugOutputStream << "\n";
            for(int i = 0; i < maxLineWidth; i++)
            {
                debugOutputStream << "=";
            }
            debugOutputStream << "\n";

        } catch (...)
        {
            // error during file writing
        }
    }
    else
    {
        // error during file opening
    }

}


MainWindow::SettingsSaveState MainWindow::saveSettings()
{
    bool errorOccured = false;

    std::ofstream settingsOutputStream;

    NikkiDebugMessage("01", "07", "Saving settings...");

    settingsFilePath = configDirPath + settingsFileName;

    //qDebug() << "settingsFilePath:\t" << settingsFilePath;
    //qDebug() << "configDir.path():\t" << configDir.path();

    if(!configDir.exists())
    {
        configDir.mkdir(configDir.path());
    }

    settingsOutputStream.open(settingsFilePath.toStdString(), std::ios::out | std::ios::binary | std::ios::trunc);

    if(settingsOutputStream.good() && settingsOutputStream.is_open())
    {
        //qDebug() << "";

        // 1: Write printAuthorInEntry
        bool tempPrintAuthorInEntry = settings.printAuthorInEntry;
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(reinterpret_cast<char*>(&tempPrintAuthorInEntry), sizeof(tempPrintAuthorInEntry));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "printAuthorInEntry not savable");
            }
        }

        // 2: Write printTimeInEntry
        bool tempPrintTimeInEntry = settings.printTimeInEntry;
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(reinterpret_cast<char*>(&tempPrintTimeInEntry), sizeof(tempPrintTimeInEntry));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "printTimeInEntry not savable");
            }
        }

        // 3: Write printLineInEntry
        bool tempPrintLineInEntry = settings.printLineInEntry;
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(reinterpret_cast<char*>(&tempPrintLineInEntry), sizeof(tempPrintLineInEntry));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "printLineInEntry not savable");
            }
        }

        // 4: Write askBeforeClose
        bool tempAskBeforeClose = settings.askBeforeClose;
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(reinterpret_cast<char*>(&tempAskBeforeClose), sizeof(tempAskBeforeClose));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "askBeforeClose not savable");
            }
        }

        // 5: Write useUnicodeEncoding
        bool tempUseUnicodeEncoding = settings.useUnicodeEncoding;
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(reinterpret_cast<char*>(&tempUseUnicodeEncoding), sizeof(tempUseUnicodeEncoding));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "useUnicodeEncoding not savable");
            }
        }

        // 6: Write useDefaultFilePath
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(reinterpret_cast<char*>(&settings.useDefaultFilePath), sizeof(settings.useDefaultFilePath));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "useDefaultFilePath not savable");
            }
        }

        // 7: Write defaultEntryPathLength
        size_t tempLength = settings.defaultEntryPath.length();
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(reinterpret_cast<char*>(&tempLength), sizeof(tempLength));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "defaultEntryPathLength not savable");
            }
        }

        // 8: Write defaultEntryPath
        if(!errorOccured)
        {
            try
            {
                settingsOutputStream.write(settings.defaultEntryPath.c_str(), tempLength);
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "07", "defaultEntryPath not savable");
            }
        }

        if(errorOccured)
        {
            NikkiDebugMessage("01", "07", "A problem occured during the saving process (settings).");

            std::stringstream temp;
            temp << "SettingsFilePath: " << settingsFilePath.toStdString();
            NikkiDebugMessage("01", "07", temp.str());

            settingsOutputStream.close();
            return SettingsSaveState::Failure;
        }
    }
    else
    {
        NikkiDebugMessage("01", "07", "Settings file could not be opened/created.");

        std::stringstream temp;
        temp << "SettingsFilePath: " << settingsFilePath.toStdString();
        NikkiDebugMessage("01", "07", temp.str());

        settingsOutputStream.close();
        return SettingsSaveState::FileOpenError;
    }

    NikkiDebugMessage("01", "07", "Settings were successfully saved.");
    settingsOutputStream.close();
    return SettingsSaveState::Success;

}

MainWindow::SettingsLoadState MainWindow::loadSettings()
{
    bool errorOccured = false;

    std::ifstream settingsInputStream;

    NikkiDebugMessage("01", "08", "Loading settings...");

    settingsFilePath = configDirPath + settingsFileName;

    //qDebug() << "settingsFilePath:\t" << settingsFilePath;

    settingsInputStream.open(settingsFilePath.toStdString(), std::ios::in | std::ios::binary);

    if(settingsInputStream.good() && settingsInputStream.is_open())
    {
        // 1: Read printAuthorInEntry
        bool tempPrintAuthorInEntry = false;
        if(!errorOccured)
        {
            try
            {
                settingsInputStream.read(reinterpret_cast<char*>(&tempPrintAuthorInEntry), sizeof(tempPrintAuthorInEntry));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "printAuthorInEntry not loadable.");
            }
        }

        // 2: Read printTimeInEntry
        bool tempPrintTimeInEntry = false;
        if(!errorOccured)
        {
            try
            {
                settingsInputStream.read(reinterpret_cast<char*>(&tempPrintTimeInEntry), sizeof(tempPrintTimeInEntry));
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "printTimeInEntry not loadable.");
            }
        }

        // 3: Read printLineInEntry
        bool tempPrintLineEntry = false;
        if(!errorOccured)
        {
            try
            {
                settingsInputStream.read(reinterpret_cast<char*>(&tempPrintLineEntry), sizeof(tempPrintLineEntry));
                //qDebug() << "Reading printLineInEntry = " << tempPrintLineEntry;
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "printLineInEntry not loadable.");
            }
        }

        // 4: Read askBeforeClose
        bool tempAskBeforeClose = false;
        if(!errorOccured)
        {
            try
            {
                settingsInputStream.read(reinterpret_cast<char*>(&tempAskBeforeClose), sizeof(tempAskBeforeClose));
                //qDebug() << "Reading tempAskBeforeClose = " << tempAskBeforeClose;
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "askBeforeClose not loadable.");
            }
        }

        // 5: Read useUnicodeEncoding
        bool tempUseUnicodeEncoding = false;
        if(!errorOccured)
        {
            try
            {
                settingsInputStream.read(reinterpret_cast<char*>(&tempUseUnicodeEncoding), sizeof(tempUseUnicodeEncoding));
                //qDebug() << "Reading tempUseUnicodeEncoding = " << tempUseUnicodeEncoding;
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "useUnicodeEncoding not loadable.");
            }
        }

        // 6: Read useDefaultFilePath
        bool tempUseDefaultFilePath = false;
        if(!errorOccured)
        {
            try
            {
                settingsInputStream.read(reinterpret_cast<char*>(&tempUseDefaultFilePath), sizeof(tempUseDefaultFilePath));
                //qDebug() << "Reading tempUseDefaultFilePath = " << tempUseDefaultFilePath;
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "useDefaultFilePath not loadable.");
            }
        }

        // 7: Read defaultEntryPathLength
        size_t tempLength = 0;
        if(!errorOccured)
        {
            try
            {
                settingsInputStream.read(reinterpret_cast<char*>(&tempLength), sizeof(tempLength));
                //qDebug() << "Reading tempLength = " << tempLength;
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "defaultEntryPathLength not loadable.");
            }
        }

        // 8: Read defaultEntryPath
        std::string tempDefaultEntryPath;
        if(!errorOccured)
        {
            try
            {
                tempDefaultEntryPath.resize(tempLength);
                settingsInputStream.read(&tempDefaultEntryPath[0], tempLength * sizeof(char));
                //qDebug() << "Reading tempDefaultEntryPath = " << QString::fromStdString(tempDefaultEntryPath);
            } catch (...)
            {
                errorOccured = true;
                NikkiDebugMessage("01", "08", "defaultEntryPath not loadable.");
            }
        }

        // Insert data into settings
        if(!errorOccured)
        {
            settings.printAuthorInEntry = tempPrintAuthorInEntry;
            settings.printTimeInEntry = tempPrintTimeInEntry;
            settings.printLineInEntry = tempPrintLineEntry;

            settings.askBeforeClose = tempAskBeforeClose;
            settings.useUnicodeEncoding = tempUseUnicodeEncoding;

            settings.useDefaultFilePath = tempUseDefaultFilePath;
            settings.defaultEntryPath = tempDefaultEntryPath;
        }
        else
        {
            NikkiDebugMessage("01", "08", "A problem occured during the loading process (settings).");

            std::stringstream temp;
            temp << "SettingsFilePath: " << settingsFilePath.toStdString();
            NikkiDebugMessage("01", "08", temp.str());

            settingsInputStream.close();
            return SettingsLoadState::Failure;
        }
    }
    else
    {
        NikkiDebugMessage("01", "08", "Settings file could not be opened.");

        std::stringstream temp;
        temp << "SettingsFilePath: " << settingsFilePath.toStdString();
        NikkiDebugMessage("01", "08", temp.str());

        settingsInputStream.close();
        return SettingsLoadState::FileOpenError;
    }

    NikkiDebugMessage("01", "08", "Settings were successfully loaded.");
    settingsInputStream.close();
    return SettingsLoadState::Success;

}


void MainWindow::doneButtonAction()
{
    doneButtonPressed = true;

    this->close();
}

void MainWindow::closeWindowAction()
{
    NikkiDebugMessage("01", "0D", "Ending program...");

    bool editAreaEmpty = ui->editArea->toPlainText().toStdString().empty();

    if(doneButtonPressed)
    {
        NikkiDebugMessage("01", "0D", "-> with Done button");

        doneButtonPressed = false;

        saveSettings();
        saveProfileData();

        if(makeNikkiEntry() != EntryWriteState::Success)
        {
            NikkiDebugMessage("01", "0D", "Error(s) occured during entry writing. Entry text was copied to clipboard.");

            QClipboard *p_Clipboard = QApplication::clipboard();
            p_Clipboard->setText(ui->editArea->toPlainText());
        }

        QApplication::quit();
    }
    else if(settings.askBeforeClose && !editAreaEmpty)
    {
        NikkiDebugMessage("01", "0D", "-> with close operation dialog");

        // Open discard- prompt
        int closeMode = CloseOperationDialog::MainCloseMode::Abort;
        CloseOperationDialog cod(&settings, &closeMode);
        cod.setModal(false);
        cod.exec();

        if(closeMode == CloseOperationDialog::MainCloseMode::KeepEntry)
        {
            NikkiDebugMessage("01", "0D", "-> keep entry");

            saveSettings();
            saveProfileData();

            if(makeNikkiEntry() != EntryWriteState::Success)
            {
                NikkiDebugMessage("01", "0D", "Error(s) occured during entry writing.");
            }

            saveErrorLog();
            QApplication::quit();
        }
        else if(closeMode == CloseOperationDialog::MainCloseMode::Discard)
        {
            NikkiDebugMessage("01", "0D", "-> discarded");

            saveSettings();
            saveProfileData();
            saveErrorLog();
            QApplication::quit();
        }
        else if (closeMode == CloseOperationDialog::MainCloseMode::Abort) {

            // qDebug() << "Aborted: Program keeps running. (Abort)";
            /*  Eigentlich sollte das Programm weiterlaufen und nur der Dialog geschlossen werden. Allerdings kann ich anscheinend
                das Beenden des Programmes (wenn auf "X" im CloseOperationDialog geklickt wird) nicht aufhalten. Deswegen wird hier erstmal
                das gleiche passieren, wie wenn man auf "Discard" klickt.
            */
            NikkiDebugMessage("01", "0D", "-> aborted (discarded)");

            saveSettings();
            saveProfileData();

            if(makeNikkiEntry() == EntryWriteState::Failure)
            {
                // Debug message ...
            }

            saveErrorLog();
            QApplication::quit();
        }
    }
}

bool MainWindow::contains_not_of_charSet(std::string stringToCheck)
{
    bool contains_not_of_charset_chars = false;
    bool isOfCharSet = false;

    static const std::string charSet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890()/*-+!§$%&/()=?#,;.:_-<>";

    // std::tolower benutzen?

    qDebug() << "String:\t" << QString::fromStdString(stringToCheck);

    for(ullong toCheckCursor = 0; toCheckCursor < stringToCheck.length(); toCheckCursor++)
    {
        isOfCharSet = false;

        for(ullong charSetCursor = 0; charSetCursor < charSet.length(); charSetCursor++)
        {
            char c = stringToCheck.at(toCheckCursor);
            char check = charSet.at(charSetCursor);

            if(c == check)
            {
                isOfCharSet = true;
                break;
            }
        }

        if(!isOfCharSet)
        {
            contains_not_of_charset_chars = true;
            break;
        }
    }

    //qDebug() << "Contains non- ASCII chars:\t" << contains_not_of_charset_chars;

    return contains_not_of_charset_chars;
}
// --[ Time functions ]--------------------------------------------------------

void MainWindow::timeUpdate(const std::tm* pTime)
{
    mainTime = pTime;
    setClockLabel();

    std::stringstream cTime;
    cTime << std::setfill('0') << std::setw(3) << mainTime->tm_yday << "|";
    cTime << std::setfill('0') << std::setw(2) << mainTime->tm_hour << ":";
    cTime << std::setfill('0') << std::setw(2) << mainTime->tm_min << ":";
    cTime << std::setfill('0') << std::setw(2) << mainTime->tm_sec;
    currentTime = cTime.str();
}

void MainWindow::setClockLabel()
{
    int hours = mainTime->tm_hour;
    int minutes = mainTime->tm_min;
    //int seconds = mainTime->tm_sec;

    int year = mainTime->tm_year + 1900;
    int month = mainTime->tm_mon + 1;
    int day = mainTime->tm_mday;

    std::stringstream timeDisplay;
    timeDisplay  << std::setfill('0') << std::setw(4) << year  << "."
                 << std::setfill('0') << std::setw(2) << month << "."
                 << std::setfill('0') << std::setw(2) << day << " | "
                 << std::setfill('0') << std::setw(2) << hours << ":"
                 << std::setfill('0') << std::setw(2) << minutes;// << ":"
    //<< std::setfill('0') << std::setw(2) << seconds;

    ui->timeLabel->setText(QString::fromStdString(timeDisplay.str()));

}

void MainWindow::NikkiDebugMessage(std::string sourceFile, std::string callingFunction, std::string message)
{
    //nikkiDebugStream << currentTime << " [01|03]:\n";
    //228|16:32:53 [01|03]:

    std::stringstream messageStream;

    messageStream << currentTime << " [" << sourceFile << "|" << callingFunction << "]:\t" << message << "\n";

    nikkiDebugStream << messageStream.str();
}

void MainWindow::NikkiDebugLine()
{
    nikkiDebugStream << "\n";
    for(int i = 0; i < maxLineWidth/2; i++)
    {
        nikkiDebugStream << "=";
    }
    nikkiDebugStream << "\n";
}

//--[ Methods ]----------------------------------------------------------------

std::basic_string<char16_t> MainWindow::utf8_to_utf16(std::string string)
{
    return QString::fromStdString(string).toStdU16String();
}

std::string utf16_to_utf8(std::basic_string<char16_t> string)
{
    return QString::fromStdU16String(string).toStdString();
}
