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
#include "settings.hpp"
#include "audio/engineid.hpp"

#include <QApplication>
#include <QDebug>
#include <QSettings>

namespace azd {
namespace Default {
    const bool ignoreMaxVolume = false;
    const bool muteOnMiddleClick = true;
#if 0
const bool showAlwaysNotifications = false;
const bool showKeyboardNotifications = true;
#endif
    const bool showOnLeftClick = true;
    const bool useAutostart = false;
    const int pageStep = 2;
} // namespace Default
} // namespace azd

azd::Settings::Settings()
    : engineId_(-1)
    , channelId_(-1)
    , pageStep_(Default::pageStep)
    , ignoreMaxVolume_(Default::ignoreMaxVolume)
    , muteOnMiddleClick_(Default::muteOnMiddleClick)
#if 0
    , showAlwaysNotifications_(Default::showAlwaysNotifications)
    , showKeyboardNotifications_(Default::showKeyboardNotifications)
#endif
    , showOnLeftClick_(Default::showOnLeftClick)
    , useAutostart_(Default::useAutostart)
    , mixerCommand_()
{
}

azd::Settings::~Settings()
{
    qDebug() << "Destroyed Settings" << Qt::endl;
}

void azd::Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    int engineId = settings.value(QStringLiteral("EngineId"), QString()).toInt();
    if (engineId < EngineId::EngineMax)
        engineId_ = engineId;

    channelId_ = settings.value(QStringLiteral("ChannelId"), -1).toInt();

    mixerCommand_ = settings.value(QStringLiteral("MixerCommand"), QString()).toString();
    ignoreMaxVolume_ = settings.value(QStringLiteral("IgnoreMaxVolume"), Default::ignoreMaxVolume).toBool();
    muteOnMiddleClick_ = settings.value(QStringLiteral("MuteOnMiddleClick"), Default::muteOnMiddleClick).toBool();
#if 0
    showAlwaysNotifications_ = settings.value(QStringLiteral("ShowAlwaysNotifications"), Default::showAlwaysNotifications).toBool();
    showKeyboardNotifications_ = settings.value(QStringLiteral("ShowKeyboardNotifications"), Default::showKeyboardNotifications).toBool();
#endif
    showOnLeftClick_ = settings.value(QStringLiteral("ShowOnLeftClick"), Default::showOnLeftClick).toBool();
    useAutostart_ = settings.value(QStringLiteral("Autostart"), Default::useAutostart).toBool();
    pageStep_ = settings.value(QStringLiteral("PageStep"), Default::pageStep).toInt();
}

void azd::Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.setValue(QStringLiteral("EngineId"), engineId_);
    settings.setValue(QStringLiteral("ChannelId"), channelId_);
    settings.setValue(QStringLiteral("PageStep"), pageStep_);
    settings.setValue(QStringLiteral("IgnoreMaxVolume"), ignoreMaxVolume_);
    settings.setValue(QStringLiteral("MuteOnMiddleClick"), muteOnMiddleClick_);
#if 0
    settings.setValue(QStringLiteral("ShowAlwaysNotifications"), showAlwaysNotifications_);
    settings.setValue(QStringLiteral("ShowKeyboardNotifications"), showKeyboardNotifications_);
#endif
    settings.setValue(QStringLiteral("ShowOnLeftClick"), showOnLeftClick_);
    settings.setValue(QStringLiteral("Autostart"), useAutostart_);
    settings.setValue(QStringLiteral("MixerCommand"), mixerCommand_);
}
