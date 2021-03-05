#include "prefsdialog.hpp"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);

    setLayout(ui->layout);
    ui->tabBehaviour->setLayout(ui->layBehaviour);
    ui->tabDevice->setLayout(ui->layDevice);
    ui->grpAudioDev->setLayout(ui->layAudioDev);
    ui->grpCmdMix->setLayout(ui->layCmdMix);
    ui->grpScroll->setLayout(ui->layScroll);
}
PrefsDialog::~PrefsDialog()
{
    delete ui;
}
