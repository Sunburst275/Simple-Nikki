#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <QUrl>

#include "vector"
#include "cstring"
#include "string.h"
#include "sstream"
#include "iostream"
#include "fstream"

#include "locale.h" // Unicode
#include "codecvt"  // Unicode

#include "timethread.h"
#include "profile.h"
#include "nikkidebug.h"
#include "settings.h"

namespace Ui {
class MainWindow;
}

// 01
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr); // 00
    ~MainWindow();

    enum class EntryWriteState{Success, Failure, FileOpenError};

    enum class SettingsLoadState{Success, Failure, FileOpenError};
    enum class SettingsSaveState{Success, Failure, FileOpenError};

    enum class ProfilesLoadState{Success, Failure, FileOpenError};
    enum class ProfilesSaveState{Success, Failure, FileOpenError};

    typedef std::basic_string<char16_t> u16string;
    typedef unsigned long long ullong;

    static constexpr int maxLineWidth = 100;
    static constexpr float versionNumber = 1.0f;

private slots:
    void on_settingsButton_released();      // 0F
    void on_doneButton_released();          // 10
    void timeUpdate(const std::tm* pTime);
    void closeEvent(QCloseEvent*);          // 0C

private:
    // MainWindow
    Ui::MainWindow *ui;

    // TimeThread
    timeThread tThread;
    const std::tm* mainTime;
    std::string currentTime;

    QDir configDir;             // Directory for profiles and settings
    QDir nikkiDir;              // Directory for profiles directories
    QDir profileDir;            // Directory for nikki entries for the respective profile

    QString currentProfile;     // Profile chosen in the profile- dropdown

    QString defaultPath;        // Default path to upper directory
    QString nikkiDirPath;       // Path to nikkiDir
    QString configDirPath;      // Path to nikki.profiles
    QString profileDirPath;         // Path to profile dir of nikki entry

    QString nikkiFilePath;      // Path to Nikki entry
    QString profilesFilePath;   // Path to nikki.profiles
    QString settingsFilePath;   // Path to nikki.settings
    QString errorLogFilePath;   // Path to nikki.errlog

    QString nikkiDirName = "/Nikkis/";          // Directory name for nikkiDir
    QString configDirName = "/NikkisConfig/";   // Directory name for configDir

    QString profilesFileName = "nikkiProfiles.np";
    QString settingsFileName = "nikkiSettings.ns";
    QString errorLogFileName = "nikkiErrLog.ne";

    std::vector<Profile> profiles;
    QString chosenProfile;

    Settings settings;

    NikkiDebug nikkiDebugStream;  // Debug stringstream // Indicators: [Window#|Func#]: (hex)

    bool doneButtonPressed = false;

private:

    void initializeProfileBox();                //  0A
    void reinitializeProfileBox();              //  11

    void initializeProgram();                   //  01
    void firstStartPrompt();                    //  0B

    ProfilesSaveState saveProfileData();        //  02
    ProfilesLoadState loadProfileData();        //  03

    EntryWriteState makeNikkiEntry();                               // 04
    EntryWriteState writeEntry();                                   // 05
    EntryWriteState writeEntry_UTF16(bool unknownProfile = false);  // 0E // 0F

    void saveErrorLog();                        //  06
    void NikkiDebugLine();
    void NikkiDebugMessage(std::string sourceFile, std::string callingFunction, std::string message);

    SettingsSaveState saveSettings();           //  07
    SettingsLoadState loadSettings();           //  08

    void doneButtonAction();                    //  0C
    void closeWindowAction();                   //  0D

    void setClockLabel();                       //  09

    std::basic_string<char16_t> utf8_to_utf16(std::string string);
    std::string utf16_to_utf8(std::basic_string<char16_t> string);

public:

    static bool contains_not_of_charSet(std::string stringToCheck);   //  12

};

/*
LAYOUT:
- Nikkis
    -> <ProfileName>
        -> <Entry>.txt
        -> ...
    -> <Another ProfileName>
        -> <Another Entry>.txt
        -> ...
- Config
    -> nikki.profiles
    -> nikki.settings
    -> nikki.errlog
*/

#endif // MAINWINDOW_H
