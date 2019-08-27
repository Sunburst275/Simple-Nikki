#ifndef FIRSTSTARTDIALOG_H
#define FIRSTSTARTDIALOG_H

#include <QDialog>
#include "profile.h"
#include "QDebug"

namespace Ui {
class firstStartDialog;
}

class firstStartDialog : public QDialog
{
    Q_OBJECT

public:
    explicit firstStartDialog(bool *useDefaultPath, QString *customPath, std::vector<profile>* profiles, QWidget *parent = nullptr);
    ~firstStartDialog();

private slots:
    void on_buttonBox_accepted();
    void on_pathBrowseButton_pressed();
    void on_pathCheckBox_stateChanged(int arg1);

private:
    void createDefaultProfile();
    void createCustomProfile();
    void closeEvent(QCloseEvent*);
    void checkCheckBoxState();
    void checkPathLineEditState();
    void initializeUI();

private:
    Ui::firstStartDialog *ui;

    std::vector<profile>* tProfiles;

    bool *useDefaultPathPtr;
    QString *customPathPtr;

    bool tempUseDefaultPath;
    QString tempCustomPath;

};

#endif // FIRSTSTARTDIALOG_H
