#ifndef CLOSEOPERATIONDIALOG_H
#define CLOSEOPERATIONDIALOG_H

#include <QDialog>
#include "settings.h"

namespace Ui {
class CloseOperationDialog;
}

class CloseOperationDialog : public QDialog
{
    Q_OBJECT

public:
    CloseOperationDialog(Settings *tSettings, int *closeMode, QWidget *parent = nullptr);
    ~CloseOperationDialog();

    enum MainCloseMode{KeepEntry, Discard, Abort};

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void closeEvent(QCloseEvent *);

private:
    Ui::CloseOperationDialog *ui;

    Settings *tempSettings;

    int internCloseMode = MainCloseMode::Abort;
    int *closeModePtr;

    void initializeUI();
    void saveSettings();
};

#endif // CLOSEOPERATIONDIALOG_H
