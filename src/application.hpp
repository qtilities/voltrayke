/*
    VolTrayke - Volume tray widget.
    Copyright (C) 2021-2023 Andrea Zanellato <redtid3@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    SPDX-License-Identifier: GPL-2.0-only
*/
#pragma once

#include "settings.hpp"

#include <QApplication>
#include <QTranslator>
#include <QSystemTrayIcon>

class AudioDevice;
class AudioEngine;

class QAction;

namespace Qtilities {

class MenuVolume;
class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int argc, char *argv[]);
    void about();
    void preferences();
    Settings &settings() { return settings_; }

private:
    bool eventFilter(QObject *, QEvent *event);

    void initLocale();
    void initUi();

    void runMixer();
    void updateDeviceList();
    void updateTrayIcon();

    void onAboutToQuit();
    void onAudioDeviceChanged(int);
    void onAudioEngineChanged(int);
    void onPrefsChanged();
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason);
    void onVolumeChanged(int);

    QStringList deviceList_;
    QTranslator qtTranslator_, translator_;
    Settings settings_;
    QSystemTrayIcon *trayIcon_;
    QAction *actAutoStart_;
    MenuVolume *mnuVolume_;
    AudioEngine *engine_;
    AudioDevice *channel_;
};
} // namespace Qtilities
