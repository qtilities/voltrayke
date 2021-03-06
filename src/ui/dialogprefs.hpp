#pragma once

#include <QDialog>

namespace Ui {
class DialogPrefs;
}
class DialogPrefs : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPrefs(QWidget *parent = nullptr);
    ~DialogPrefs();

private:
    Ui::DialogPrefs *ui;
};

