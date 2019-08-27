#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>

#include "cstring"
#include "string"
#include "sstream"
#include "iostream"
#include "fstream"

#include "profile.h"
#include "nikkidebug.h"
#include "settings.h"
#include "timethread.h"
#include "errlogerasedialog.h"

namespace Ui {
class SettingsWindow;
}

// 02
class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(timeThread *tThread, std::vector<Profile> *profiles, NikkiDebug *nikkiDebugStream, Settings *settings, QDir* configDir, const float versionNumber, QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void timeUpdate(const std::tm* pTime);

    void on_buttonBox_accepted();                                       // 0A
    void on_buttonBox_rejected();                                       // 0B

    void on_addProfileButton_released();                                // 0C
    void on_makeDefaultProfileButton_released();                        // 0D
    void on_removeProfileButton_released();                             // 0E

    void on_printSeperationLineCheckBox_stateChanged(int arg1);         // 0F
    void on_useDefaultCheckBox_stateChanged(int arg1);                  // 10
    void on_closeCommandCheckBox_stateChanged(int arg1);                // 11
    void on_nikkisPathBrowseButton_released();                          // 12
    void on_printTimeCheckBox_stateChanged(int arg1);                   // 13
    void on_printAuthorCheckBox_stateChanged(int arg1);                 // 14
    void on_unicodeCheckBox_stateChanged(int arg1);                     // 15

    void on_helpComboBox_currentIndexChanged(const QString &arg1);      // 16

    void on_Sunburst275Button_released();                               // 17
    void on_walkthroughButton_released();                               // 18
    void on_feedbackButton_released();                                  // 19

    void on_openErrLogFileButton_released();                            // 1A
    void on_openFileLocationButton_released();                          // 1B
    void on_eraseFileButton_released();                                 // 1C
    void eraseErrlog();                                                 // 1D

private:
    Ui::SettingsWindow *ui;

    // ConfigDir
    QDir* configDirPtr;

    // Time
    const std::tm *tempTime = nullptr;
    std::string tempCurrentTime;

    // EraseErrlogFileStream
    std::ofstream tempErrlogStream;

    // Settings
    settings tempSettings;
    settings *settings = nullptr;

    // Profiles
    std::vector<Profile> tempProfiles = {};
    std::vector<Profile>* profiles = nullptr;

    // Debug errorlog
    NikkiDebug* nikkiDebugStream;

    const float versionNumber = 0;

private:
    void initializeSettings();                                                                          // 01
    void reinitializeSettings();                                                                        // 02

    void initializeProfiles();                                                                          // 03
    void reinitializeProfiles();                                                                        // 04

    void initializeHelp();                                                                              // 05
    void initializeDebugPrompt();                                                                       // 06
    void NikkiDebugMessage(std::string sourceFile, std::string callingFunction, std::string message);   // 07

    void createNewProfile(QString name, bool makeDefault = false);                                      // 08
    void setVersionLabel();                                                                             // 09
};

#endif // SETTINGSWINDOW_H
