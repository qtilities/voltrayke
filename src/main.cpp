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
#include "src/ui/volumewidget.hpp"

#include <QApplication>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setQuitOnLastWindowClosed(false);

    app.setOrganizationName("AZDrums");
    app.setOrganizationDomain("azdrums.org");
    app.setApplicationName("qvolume");
    app.setApplicationDisplayName("QVolume");

    QIcon icon = QIcon::fromTheme("audio-volume-medium");
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(icon);
    trayIcon->setVisible(true);

    VolumeMenu w(trayIcon);
    w.setWindowIcon(icon);

    return app.exec();
}
