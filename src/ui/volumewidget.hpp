/*
    Copyright (C) 2020-2021 Andrea Zanellato <redtid3@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    For a full copy of the GNU General Public License see the LICENSE file
*/
#pragma once

#include <QWidget>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class VolumeMenu; }
QT_END_NAMESPACE

class QMenu;
class QSystemTrayIcon;
class PrefsDialog;
class VolumeMenu : public QWidget
{
    Q_OBJECT

public:
    VolumeMenu(QSystemTrayIcon *icon, QWidget *parent = nullptr);
    ~VolumeMenu();

private:
    void showEvent(QShowEvent *) override;

    void onAboutToQuit();
    void onAboutClicked();
    void onPrefsClicked();

    void loadSettings();
    void saveSettings();

    void createAutostartFile();
    void deleteAutostartFile();

    void setX11NetWmState();

    Ui::VolumeMenu *ui;
    PrefsDialog     *dlgPrefs;
    QAction         *actAutoStart,
                    *actAbout,
                    *actPrefs,
                    *actQuit;
    QMenu           *trayMenu;
    QSystemTrayIcon *trayIcon;
};
