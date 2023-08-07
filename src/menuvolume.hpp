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

#include <QMenu>

class QCheckBox;
class QLabel;
class QSlider;

namespace Qtilities {

class MenuVolume : public QMenu {
    Q_OBJECT

public:
    MenuVolume(QWidget* parent = nullptr);

    void loadSettings();
    void popUp();
    void setMute(bool);
    void setVolume(int);

signals:
    void sigRunMixer();
    void sigMuteToggled(bool);
    void sigVolumeChanged(int);

private:
    QCheckBox *chkMute_;
    QLabel *lblVolume_;
    QSlider *sldVolume_;
};
} // namespace Qtilities
