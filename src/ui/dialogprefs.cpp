/*
    VolTrayke - Volume tray widget.
    Copyright (C) 2021  Andrea Zanellato <redtid3@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

    SPDX-License-Identifier: GPL-2.0-only
*/
#include "application.hpp"
#include "audio/engineid.hpp"

#include "dialogprefs.hpp"
#include "ui_dialogprefs.h"

#include <QCloseEvent>
#include <QPushButton>

#include <QDebug>

azd::DialogPrefs::DialogPrefs(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogPrefs)
{
    ui->setupUi(this);

#ifdef USE_ALSA
    ui->cbxEngine->addItem("ALSA");
#endif
#ifdef USE_PULSEAUDIO
    ui->cbxEngine->addItem("PulseAudio");
#endif
    // TODO: LXQt volume plugin has these features that I don't use,
    // here only as reminder if someone ever request to enable them.
#if 0
    ui->chkIgnoreMax->setEnabled(ui->cbxEngine->currentText() == "PulseAudio");
#else
    ui->tbwPrefs->removeTab(2); // Notifications
    ui->chkIgnoreMax->setVisible(false);
    ui->chkShowOnClick->setVisible(false);

    // TODO: PNMixer use double values
    ui->lblStep->setVisible(false);
    ui->sbxStep->setVisible(false);
#endif
    connect(ui->cbxEngine, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int id) {
                ui->chkIgnoreMax->setEnabled(id == EngineId::PulseAudio);
            });

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &DialogPrefs::onPrefsChanged);

    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::hide);
}

azd::DialogPrefs::~DialogPrefs()
{
    delete ui;
    qDebug() << "Destroyed DialogPrefs" << Qt::endl;
}

void azd::DialogPrefs::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}

void azd::DialogPrefs::loadSettings()
{
    Settings& settings = static_cast<VolTrayke*>(qApp)->settings();
    ui->cbxChannel->setCurrentIndex(settings.channelId());
    ui->cbxEngine->setCurrentIndex(settings.engineId());
    ui->chkIgnoreMax->setChecked(settings.ignoreMaxVolume());
    ui->chkMuteOnMiddleClick->setChecked(settings.muteOnMiddleClick());
    ui->chkShowOnClick->setChecked(settings.showOnLeftClick());
    ui->sbxPageStep->setValue(settings.pageStep());
    ui->txtMixerCmd->setText(settings.mixerCommand());
}

void azd::DialogPrefs::saveSettings()
{
    Settings& settings = static_cast<VolTrayke*>(qApp)->settings();
    settings.setChannelId(ui->cbxChannel->currentIndex());
    settings.setEngineId(ui->cbxEngine->currentIndex());
    settings.setIgnoreMaxVolume(ui->chkIgnoreMax->isChecked());
    settings.setMuteOnMiddleClick(ui->chkMuteOnMiddleClick->isChecked());
    settings.setShowOnLeftClick(ui->chkShowOnClick->isChecked());
    settings.setPageStep(ui->sbxPageStep->value());
    settings.setMixerCommand(ui->txtMixerCmd->text());
}

void azd::DialogPrefs::setDeviceList(const QStringList& list)
{
    ui->cbxChannel->clear();
    ui->cbxChannel->addItems(list);
}

void azd::DialogPrefs::onPrefsChanged()
{
    Settings& settings = static_cast<VolTrayke*>(qApp)->settings();
    int engineIndex = ui->cbxEngine->currentIndex();
    int channelIndex = ui->cbxChannel->currentIndex();

    if (settings.engineId() != engineIndex) {
        settings.setEngineId(engineIndex);
        emit sigAudioEngineChanged(engineIndex);
    }
    if (settings.channelId() != channelIndex) {
        settings.setChannelId(channelIndex);
        emit sigAudioDeviceChanged(channelIndex);
    }
    settings.setIgnoreMaxVolume(ui->chkIgnoreMax->isChecked());
    settings.setMixerCommand(ui->txtMixerCmd->text());
    settings.setMuteOnMiddleClick(ui->chkMuteOnMiddleClick->isChecked());
    settings.setPageStep(ui->sbxPageStep->value());
    settings.setShowOnLeftClick(ui->chkShowOnClick->isChecked());

    hide();
}
