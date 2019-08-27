#ifndef ERRLOGERASEDIALOG_H
#define ERRLOGERASEDIALOG_H

#include <QDialog>

namespace Ui {
class ErrlogEraseDialog;
}

class ErrlogEraseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrlogEraseDialog(bool *reallyErase, QWidget *parent = nullptr);
    ~ErrlogEraseDialog();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::ErrlogEraseDialog *ui;

    bool *reallyErasePtr;

};

#endif // ERRLOGERASEDIALOG_H
