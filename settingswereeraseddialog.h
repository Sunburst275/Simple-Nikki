#ifndef SETTINGSWEREERASEDDIALOG_H
#define SETTINGSWEREERASEDDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsWereErasedDialog;
}

class SettingsWereErasedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWereErasedDialog(QWidget *parent = nullptr);
    ~SettingsWereErasedDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SettingsWereErasedDialog *ui;
};

#endif // SETTINGSWEREERASEDDIALOG_H
