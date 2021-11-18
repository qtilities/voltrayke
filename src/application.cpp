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
#include "ui/dialogabout.hpp"
#include "ui/dialogprefs.hpp"
#include "src/ui/menuvolume.hpp"

#include <QAction>
#include <QCursor>
#include <QDir>
#include <QScreen>
#include <QStandardPaths>
#include <QTextStream>
#include <QTranslator>

//==============================================================================
// Utilities
//==============================================================================
QScreen* screenAt(const QPoint& pos)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return qApp->screenAt(pos);
#else
    const QList<QScreen*> screens = QGuiApplication::screens();
    for (QScreen* screen : screens) {
        if (screen->geometry().contains(pos))
            return screen;
    }
    return nullptr;
#endif
}

void centerOnScreen(QWidget* widget)
{
    if (const QScreen* screen = screenAt(QCursor::pos())) {
        QRect rct = screen->geometry();
        widget->move((rct.width() - widget->width()) / 2,
                     (rct.height() - widget->height()) / 2);
    }
}
void createAutostartFile()
{
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QString appName = QApplication::applicationName();
    QString filePath = configDir.filePath("autostart/" + appName + ".desktop");
    QFile file(filePath);

    if (file.exists() || !file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "[Desktop Entry]\n";
    out << "Name=" + QApplication::applicationDisplayName() + "\n";
    out << "Type=Application\n";
    out << "Exec=" + appName + "\n";
    out << "Icon=" + appName + "\n";
    out << "Terminal=false\n";
}
void deleteAutostartFile()
{
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QString filePath(configDir.filePath("autostart/" + QApplication::applicationName() + ".desktop"));
    QFile file(filePath);

    if (!file.exists())
        return;

    file.remove();
}
//==============================================================================
// Application
//==============================================================================
azd::VolTrayke::VolTrayke(int& argc, char* argv[])
    : QApplication(argc, argv)
    , dlgAbout_(new DialogAbout)
    , dlgPrefs_(new DialogPrefs)
    , mnuVolume(new MenuVolume)
    , trayIcon(new QSystemTrayIcon(QIcon::fromTheme("audio-volume-medium")))
{
    setOrganizationName("AZDrums");
    setOrganizationDomain("azdrums.org");
    setApplicationName("voltrayke");
    setApplicationDisplayName("VolTrayke");

    QAction* actAbout = new QAction(QIcon::fromTheme("help-about"), tr("&About"), this);
    QAction* actPrefs = new QAction(QIcon::fromTheme("preferences-system"), tr("&Preferences"), this);
    QAction* actQuit = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), this);

    QMenu* mnuActions = new QMenu();
    mnuActions->addAction(actAutoStart_);
    mnuActions->addAction(actPrefs);
    mnuActions->addAction(actAbout);
    mnuActions->addAction(actQuit);

    trayIcon->setContextMenu(mnuActions);
    trayIcon->show();

    connect(actAbout, &QAction::triggered,
            this, [=] { if (dlgAbout_->isHidden()) dlgAbout_->show(); });

    connect(actPrefs, &QAction::triggered,
            this, [=] { if (dlgPrefs_->isHidden()) dlgPrefs_->show(); });

    connect(actQuit, &QAction::triggered, this, &VolTrayke::quit);

    connect(this, &QApplication::aboutToQuit, mnuActions, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, mnuVolume, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, trayIcon, &QObject::deleteLater);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &VolTrayke::onIconActivated);
}
void azd::VolTrayke::onIconActivated(QSystemTrayIcon::ActivationReason reason)
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
    azd::VolTrayke app(argc, argv);

    QTranslator translator;
    if (translator.load(QLocale(), QLatin1String("voltrayke"),
                        QLatin1String("_"),
                        QLatin1String(":/translations")))
        app.installTranslator(&translator);

    return app.exec();
}
