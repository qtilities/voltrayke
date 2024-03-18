/*
    VolTrayke - Volume tray widget.
    Copyright (C) 2021-2024 Andrea Zanellato <redtid3@gmail.com>

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

#include <QDialog>


namespace Qtilities {

namespace Ui {
class DialogPrefs;
}
class DialogPrefs : public QDialog {
    Q_OBJECT

public:
    explicit DialogPrefs(QWidget* parent = nullptr);
    ~DialogPrefs();

    void loadSettings();
    void setDeviceList(const QStringList&);

private:
    void accept() override;
    void setupUi();

    Qtilities::Ui::DialogPrefs *ui;
};
} // namespace Qtilities
