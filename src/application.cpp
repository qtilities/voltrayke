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
#include "src/application.hpp"
#include "src/ui/menuactions.hpp"
#include "src/ui/menuvolume.hpp"

#include <QCursor>
#include <QTranslator>

VolTrayke::VolTrayke(int& argc, char* argv[])
    : QApplication(argc, argv)
    , mnuActions(new MenuActions)
    , mnuVolume(new MenuVolume)
    , trayIcon(new QSystemTrayIcon(QIcon::fromTheme("audio-volume-medium")))
{
    setOrganizationName("AZDrums");
    setOrganizationDomain("azdrums.org");
    setApplicationName("voltrayke");
    setApplicationDisplayName("VolTrayke");

    trayIcon->setContextMenu(mnuActions);
    trayIcon->show();

    connect(this, &QApplication::aboutToQuit, mnuActions, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, mnuVolume, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, trayIcon, &QObject::deleteLater);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &VolTrayke::onIconActivated);
}
void VolTrayke::onIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
        mnuVolume->show();

        QPoint pos = QCursor::pos();
        pos.setX(pos.x() - mnuVolume->width() / 2);

        mnuVolume->adjustSize();
        mnuVolume->popup(pos);
    }
}
int main(int argc, char* argv[])
{
    VolTrayke app(argc, argv);

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("voltrayke"),
                        QLatin1String("_"),
                        QLatin1String(":/translations")))
        app.installTranslator(&translator);

    return app.exec();
}
