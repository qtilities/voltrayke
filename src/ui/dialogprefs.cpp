#include "src/ui/dialogprefs.hpp"
#include "ui_dialogprefs.h"

DialogPrefs::DialogPrefs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPrefs)
{
    ui->setupUi(this);

    setLayout(ui->layout);
    ui->tabBehaviour->setLayout(ui->layBehaviour);
    ui->tabDevice->setLayout(ui->layDevice);
    ui->grpAudioDev->setLayout(ui->layAudioDev);
    ui->grpCmdMix->setLayout(ui->layCmdMix);
    ui->grpScroll->setLayout(ui->layScroll);
}
DialogPrefs::~DialogPrefs()
{
    delete ui;
}
