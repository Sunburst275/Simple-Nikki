#ifndef ERRORLOGWASERASEDDIALOG_H
#define ERRORLOGWASERASEDDIALOG_H

#include <QDialog>

namespace Ui {
class ErrorlogWasErasedDialog;
}

class ErrorlogWasErasedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorlogWasErasedDialog(QWidget *parent = nullptr);
    ~ErrorlogWasErasedDialog();

private:
    Ui::ErrorlogWasErasedDialog *ui;
};

#endif // ERRORLOGWASERASEDDIALOG_H
