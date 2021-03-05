#pragma once

#include <QDialog>

namespace Ui {
class PrefsDialog;
}
class PrefsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrefsDialog(QWidget *parent = nullptr);
    ~PrefsDialog();

private:
    Ui::PrefsDialog *ui;
};

