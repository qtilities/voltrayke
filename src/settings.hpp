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
#pragma once

#include <QString>

namespace Qtilities {

class Settings {
public:
    Settings();
    ~Settings();

    void load();
    void save();

    int engineId() const { return engineId_; }
    void setEngineId(int id) { engineId_ = id; }

    int channelId() const { return channelId_; }
    void setChannelId(int id) { channelId_ = id; }

    double pageStep() { return pageStep_; }
    void setPageStep(double step) { pageStep_ = step; }

    double singleStep() { return singleStep_; }
    void setSingleStep(double step) { singleStep_ = step; }

    bool isNormalized() const { return isNormalized_; }
    void setNormalized(bool normalized) { isNormalized_ = normalized; }

    bool muteOnMiddleClick() const { return muteOnMiddleClick_; }
    void setMuteOnMiddleClick(bool mute) { muteOnMiddleClick_ = mute; }

    bool useAutostart() const { return useAutostart_; }
    void setUseAutostart(bool autostart) { useAutostart_ = autostart; }

    QString mixerCommand() const { return mixerCommand_; }
    void setMixerCommand(const QString& command) { mixerCommand_ = command; }
#if 0
    bool ignoreMaxVolume() const { return ignoreMaxVolume_; }
    void setIgnoreMaxVolume(bool ignore) { ignoreMaxVolume_ = ignore; }

    bool showAlwaysNotifications() const { return showAlwaysNotifications_; }
    void setShowAlwaysNotifications(bool show) { showAlwaysNotifications_ = show; }

    bool showKeyboardNotifications() const { return showKeyboardNotifications_; }
    void setShowKeyboardNotifications(bool show) { showKeyboardNotifications_ = show; }

    bool showOnLeftClick() const { return showOnLeftClick_; }
    void setShowOnLeftClick(bool show) { showOnLeftClick_ = show; }
#endif

private:
    int engineId_;
    int channelId_;
    double pageStep_;
    double singleStep_;
    bool isNormalized_;
    bool muteOnMiddleClick_;
#if 0
    bool ignoreMaxVolume_;
    bool showAlwaysNotifications_;
    bool showKeyboardNotifications_;
    bool showOnLeftClick_;
#endif
    bool useAutostart_;
    QString mixerCommand_;
};
} // namespace azd
