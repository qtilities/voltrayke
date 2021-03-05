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
#include "src/ui/prefsdialog.hpp"
#include "src/ui/volumewidget.hpp"
#include "./ui_volumewidget.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QCloseEvent>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include <QSystemTrayIcon>
#include <QX11Info>

#include <X11/Xlib-xcb.h>
#include <X11/Xatom.h>

VolumeMenu::VolumeMenu(QSystemTrayIcon *icon, QWidget *parent)
    : QWidget(parent)
    , ui          (new Ui::VolumeMenu)
    , dlgPrefs    (new PrefsDialog)
    , actAutoStart(new QAction(tr("Auto&start"), this))
    , actAbout    (new QAction(QIcon::fromTheme("help-about"),         tr("&About"), this))
    , actPrefs    (new QAction(QIcon::fromTheme("preferences-system"), tr("&Preferences"), this))
    , actQuit     (new QAction(QIcon::fromTheme("application-exit"),   tr("&Quit"), this))
    , trayMenu    (new QMenu(this))
    , trayIcon    (icon)
{
    ui->setupUi(this);

    actAutoStart->setCheckable(true);

    trayMenu->addAction(actAutoStart);
    trayMenu->addAction(actPrefs);
    trayMenu->addAction(actAbout);
    trayMenu->addAction(actQuit);

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    connect(qApp,     &QCoreApplication::aboutToQuit, this, &VolumeMenu::onAboutToQuit);
    connect(actPrefs, &QAction::triggered,            this, &VolumeMenu::onPrefsClicked);
    connect(actAbout, &QAction::triggered,            this, &VolumeMenu::onAboutClicked);
    connect(actQuit,  &QAction::triggered,            qApp, &QCoreApplication::quit);

    connect(trayIcon, &QSystemTrayIcon::activated,
    [=](QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
        {
            setVisible(!isVisible());
        }
    });
    dlgPrefs->setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlag(Qt::Tool);
    setWindowFlag(Qt::FramelessWindowHint);
    loadSettings();
}
VolumeMenu::~VolumeMenu()
{
    delete ui;
}
void VolumeMenu::showEvent(QShowEvent *)
{
    setX11NetWmState();
    QRect trayRect = trayIcon->geometry();
    move(trayRect.x(), trayRect.y());
}
void VolumeMenu::onAboutToQuit()
{
    actAutoStart->isChecked() ? createAutostartFile() : deleteAutostartFile();
    saveSettings();
}
void VolumeMenu::onAboutClicked()
{
    QFile f(":/about.html");
    if (!f.open(QFile::ReadOnly | QFile::Text))
    {
        qDebug() << tr("Error loading about file") << Qt::endl;
        return;
    }
    QTextStream in(&f);
    QString text = in.readAll();
    f.close();
    QMessageBox::about(this, tr("About"), text);
}
void VolumeMenu::onPrefsClicked()
{
    if (dlgPrefs->isHidden())
        dlgPrefs->show();
}
void VolumeMenu::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.beginGroup("Main");
    actAutoStart->setChecked(settings.value("AutoStart", false).toBool());
    settings.endGroup();
}
void VolumeMenu::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationDisplayName());

    settings.beginGroup("Main");
    settings.setValue("AutoStart", actAutoStart->isChecked());
    settings.endGroup();
}
void VolumeMenu::createAutostartFile()
{
    QDir    configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QString appName  = QApplication::applicationName();
    QString filePath = configDir.filePath("autostart/" + appName + ".desktop");
    QFile   file(filePath);

    if (file.exists() || !file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "[Desktop Entry]\n";
    out << "Name=" + QApplication::applicationDisplayName() + "\n";
    out << "Type=Application\n";
    out << "Exec=" + appName + "\n";
    out << "Terminal=false\n";
}
void VolumeMenu::deleteAutostartFile()
{
    QDir configDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation));
    QString filePath(configDir.filePath("autostart/" + QApplication::applicationName() + ".desktop"));
    QFile file(filePath);

    if (!file.exists())
        return;

    file.remove();
}
void VolumeMenu::setX11NetWmState()
{
    // Needed for some WMs to avoid to display the window in the taskbar
    // and task switcher and display it in all desktop (FIXME)
    Display *display = QX11Info::display();
    WId     window   = this->winId();
    Atom    state    = XInternAtom(display, "_NET_WM_STATE", false);
    Atom    sticky   = XInternAtom(display, "_NET_WM_STATE_STICKY", false);
    Atom    skipPgr  = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", false);
    Atom    skipTbr  = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", false);
    XChangeProperty(display, window, state, XA_ATOM, 32, PropModeAppend, (unsigned char*) &sticky, 1L);
    XChangeProperty(display, window, state, XA_ATOM, 32, PropModeAppend, (unsigned char*) &skipPgr, 1L);
    XChangeProperty(display, window, state, XA_ATOM, 32, PropModeAppend, (unsigned char*) &skipTbr, 1L);
}
