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

namespace Qtilities {
namespace Default {
    const bool isNormalized = true;
    const bool muteOnMiddleClick = true;
    const bool useAutostart = false;
    const double pageStep = 2.00;
    const double singleStep = 1.00;
#if 0
    const bool ignoreMaxVolume = false;
    const bool showAlwaysNotifications = false;
    const bool showKeyboardNotifications = true;
    const bool showOnLeftClick = true;
#endif
} // namespace Default
} // namespace Qtilities

Qtilities::Settings::Settings()
    : engineId_(-1)
    , channelId_(-1)
    , pageStep_(Default::pageStep)
    , singleStep_(Default::singleStep)
    , isNormalized_(Default::isNormalized)
    , muteOnMiddleClick_(Default::muteOnMiddleClick)
    , useAutostart_(Default::useAutostart)
    , mixerCommand_()
#if 0
    , ignoreMaxVolume_(Default::ignoreMaxVolume)
    , showAlwaysNotifications_(Default::showAlwaysNotifications)
    , showKeyboardNotifications_(Default::showKeyboardNotifications)
    , showOnLeftClick_(Default::showOnLeftClick)
#endif
{
}

Qtilities::Settings::~Settings()
{
    qDebug() << "Destroyed Settings";
}

void Qtilities::Settings::load()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    int engineId = settings.value(QStringLiteral("EngineId"), QString()).toInt();
    if (engineId < EngineId::EngineMax)
        engineId_ = engineId;

    channelId_ = settings.value(QStringLiteral("ChannelId"), -1).toInt();

    mixerCommand_ = settings.value(QStringLiteral("MixerCommand"), QString()).toString();
    isNormalized_ = settings.value(QStringLiteral("IsNormalized"), Default::isNormalized).toBool();
    muteOnMiddleClick_ = settings.value(QStringLiteral("MuteOnMiddleClick"), Default::muteOnMiddleClick).toBool();
    useAutostart_ = settings.value(QStringLiteral("Autostart"), Default::useAutostart).toBool();
    pageStep_ = settings.value(QStringLiteral("PageStep"), Default::pageStep).toDouble();
    singleStep_ = settings.value(QStringLiteral("SingleStep"), Default::singleStep).toDouble();
#if 0
    ignoreMaxVolume_ = settings.value(QStringLiteral("IgnoreMaxVolume"), Default::ignoreMaxVolume).toBool();
    showOnLeftClick_ = settings.value(QStringLiteral("ShowOnLeftClick"), Default::showOnLeftClick).toBool();
    showAlwaysNotifications_ = settings.value(QStringLiteral("ShowAlwaysNotifications"), Default::showAlwaysNotifications).toBool();
    showKeyboardNotifications_ = settings.value(QStringLiteral("ShowKeyboardNotifications"), Default::showKeyboardNotifications).toBool();
#endif
}

void Qtilities::Settings::save()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.setValue(QStringLiteral("EngineId"), engineId_);
    settings.setValue(QStringLiteral("ChannelId"), channelId_);
    settings.setValue(QStringLiteral("PageStep"), pageStep_);
    settings.setValue(QStringLiteral("SingleStep"), singleStep_);
    settings.setValue(QStringLiteral("IsNormalized"), isNormalized_);
    settings.setValue(QStringLiteral("MuteOnMiddleClick"), muteOnMiddleClick_);
    settings.setValue(QStringLiteral("Autostart"), useAutostart_);
    settings.setValue(QStringLiteral("MixerCommand"), mixerCommand_);
#if 0
    settings.setValue(QStringLiteral("IgnoreMaxVolume"), ignoreMaxVolume_);
    settings.setValue(QStringLiteral("ShowAlwaysNotifications"), showAlwaysNotifications_);
    settings.setValue(QStringLiteral("ShowKeyboardNotifications"), showKeyboardNotifications_);
    settings.setValue(QStringLiteral("ShowOnLeftClick"), showOnLeftClick_);
#endif
}
