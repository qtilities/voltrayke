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

Qtilities::DialogPrefs::DialogPrefs(QWidget* parent)
    : QDialog(parent)
    , ui(new Qtilities::Ui::DialogPrefs)
{
    ui->setupUi(this);

#if USE_ALSA
    ui->cbxEngine->addItem("ALSA");
#endif
#if USE_PULSEAUDIO
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
#endif
    connect(ui->cbxEngine, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int id) {
                ui->chkIgnoreMax->setEnabled(id == EngineId::PulseAudio);
                ui->chkNormalize->setEnabled(id == EngineId::Alsa);
            });
    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &DialogPrefs::onPrefsChanged);

    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::hide);
}

Qtilities::DialogPrefs::~DialogPrefs()
{
    delete ui;
    qDebug() << "Destroyed DialogPrefs";
}

void Qtilities::DialogPrefs::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}

void Qtilities::DialogPrefs::loadSettings()
{
    Settings& settings = static_cast<Application*>(qApp)->settings();
    ui->cbxEngine->setCurrentIndex(settings.engineId());
    ui->cbxChannel->setCurrentIndex(settings.channelId());
    ui->chkNormalize->setChecked(settings.isNormalized());
    ui->chkMuteOnMiddleClick->setChecked(settings.muteOnMiddleClick());
    ui->sbxPageStep->setValue(settings.pageStep());
    ui->sbxStep->setValue(settings.singleStep());
    ui->txtMixerCmd->setText(settings.mixerCommand());
#if 0
    ui->chkIgnoreMax->setChecked(settings.ignoreMaxVolume());
    ui->chkShowOnClick->setChecked(settings.showOnLeftClick());
#endif
}

void Qtilities::DialogPrefs::saveSettings()
{
    Settings& settings = static_cast<Application*>(qApp)->settings();
    settings.setEngineId(ui->cbxEngine->currentIndex());
    settings.setChannelId(ui->cbxChannel->currentIndex());
    settings.setNormalized(ui->chkNormalize->isChecked());
    settings.setMuteOnMiddleClick(ui->chkMuteOnMiddleClick->isChecked());
    settings.setPageStep(ui->sbxPageStep->value());
    settings.setSingleStep(ui->sbxStep->value());
    settings.setMixerCommand(ui->txtMixerCmd->text());
#if 0
    settings.setIgnoreMaxVolume(ui->chkIgnoreMax->isChecked());
    settings.setShowOnLeftClick(ui->chkShowOnClick->isChecked());
#endif
}

void Qtilities::DialogPrefs::setDeviceList(const QStringList& list)
{
    ui->cbxChannel->clear();
    ui->cbxChannel->addItems(list);
}

void Qtilities::DialogPrefs::onPrefsChanged()
{
    Settings& settings = static_cast<Application*>(qApp)->settings();
    int previousBackendId = settings.engineId();
    int previousChannelId = settings.channelId();

    saveSettings();
    emit sigPrefsChanged();

    int currentBackendId = settings.engineId();
    int currentChannelId = settings.channelId();

    if (currentBackendId != previousBackendId)
        emit sigEngineChanged(currentBackendId);

    if (currentChannelId != previousChannelId)
        emit sigChannelChanged(currentChannelId);

    hide();
}
