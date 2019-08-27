#ifndef ADDPROFILEDIALOG_H
#define ADDPROFILEDIALOG_H

#include <QDialog>

namespace Ui {
class addProfileDialog;
}

class addProfileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addProfileDialog(QString* tempNewProfileName, bool* makeDefault, QWidget *parent = nullptr);
    ~addProfileDialog();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::addProfileDialog *ui;

    bool* makeDefaultPtr;
    QString* profileNamePtr;
};

#endif // ADDPROFILEDIALOG_H
