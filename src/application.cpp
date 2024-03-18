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
#include "application.hpp"
#include "dialogabout.hpp"
#include "dialogprefs.hpp"
#include "menuvolume.hpp"
#include "qtilities.hpp"

#include "audio/device.hpp"
#if USE_ALSA
#include "audio/device/alsa.hpp"
#include "audio/engine/alsa.hpp"
#endif
#if USE_PULSEAUDIO
#include "audio/engine/pulseaudio.hpp"
#endif

#if QT_VERSION < 0x060000
    #include <StatusNotifierItemQt5/statusnotifieritem.h>
#else
    #include <StatusNotifierItemQt6/statusnotifieritem.h>
#endif

#include <QAction>
#include <QIcon>
#include <QLibraryInfo>
#include <QMenu>
#include <QProcess>
#include <QToolTip>
#include <QWheelEvent>

#include <QDebug>

Qtilities::Application::Application(int argc, char* argv[])
    : QApplication(argc, argv)
    , engine_(nullptr)
    , channel_(nullptr)
{
    setOrganizationName(ORGANIZATION_NAME);
    setOrganizationDomain(ORGANIZATION_DOMAIN);
    setApplicationName(APPLICATION_NAME);
    setApplicationDisplayName(APPLICATION_DISPLAY_NAME);

    trayIcon_ = new StatusNotifierItem(qApp->applicationName(), this);
    trayIcon_->setCategory(StatusNotifierItem::SNICategory::ApplicationStatus);
    trayIcon_->setStatus(StatusNotifierItem::SNIStatus::Active);
    trayIcon_->setToolTipTitle(qApp->applicationDisplayName());

    setQuitOnLastWindowClosed(false);

    initLocale();
    initUi();
}

void Qtilities::Application::initLocale()
{
#if 1
    QLocale locale = QLocale::system();
#else
    QLocale locale(QLocale("it"));
    QLocale::setDefault(locale);
#endif
    // install the translations built into Qt itself
    if (qtTranslator_.load(QStringLiteral("qt_") + locale.name(),
#if QT_VERSION < 0x060000
                           QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
#else
                           QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
#endif
        installTranslator(&qtTranslator_);

    // E.g. "<appname>_en"
    QString translationsFileName = QCoreApplication::applicationName().toLower() + '_' + locale.name();
    // Try first in the same binary directory, in case we are building,
    // otherwise read from system data
    QString translationsPath = QCoreApplication::applicationDirPath();

    bool isLoaded = translator_.load(translationsFileName, translationsPath);
    if (!isLoaded) {
        // "/usr/share/<appname>/translations
        isLoaded = translator_.load(translationsFileName,
                                    QStringLiteral(PROJECT_DATA_DIR) + QStringLiteral("/translations"));
    }
    if (isLoaded)
        installTranslator(&translator_);
}

void Qtilities::Application::initUi()
{
    settings_.load();

    actAutoStart_ = new QAction(tr("Auto&start"), this);
    mnuVolume_ = new MenuVolume;
    mnuVolume_->loadSettings();
    mnuVolume_->show(); // FIXME: crash without this
    mnuVolume_->hide();

    onAudioEngineChanged(settings_.engineId());
    onAudioDeviceChanged(settings_.channelId());
    updateDeviceList();
    updateTrayIcon();

    if(channel_) {
        channel_->setMute(settings_.isMuted());
        int volume = std::clamp(settings_.volume(), 0, 100);
        channel_->setVolume(volume);
        mnuVolume_->setVolume(channel_->volume());
    }
    actAutoStart_->setCheckable(true);
    actAutoStart_->setChecked(settings_.useAutostart());

    QAction *actAbout
        = new QAction(QIcon::fromTheme("help-about", QIcon(":/help-about")), tr("&About"), this);
    QAction *actPrefs
        = new QAction(QIcon::fromTheme("preferences-system", QIcon(":/preferences-system")),
                      tr("&Preferences"), this);
    QAction *actQuit = new QAction(QIcon::fromTheme("application-exit", QIcon(":/application-exit")),
                                   tr("&Quit"), this);

    QMenu* mnuActions = new QMenu(mnuVolume_);
    mnuActions->addAction(actAutoStart_);
    mnuActions->addAction(actPrefs);
    mnuActions->addAction(actAbout);
    mnuActions->addAction(actQuit);

    trayIcon_->setContextMenu(mnuActions);

    connect(actAbout, &QAction::triggered, this, &Application::about);
    connect(actPrefs, &QAction::triggered, this, &Application::preferences);
    connect(actQuit, &QAction::triggered, this, &Application::quit);

    connect(this, &QApplication::aboutToQuit, mnuVolume_, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, trayIcon_, &QObject::deleteLater);
    connect(this, &QApplication::aboutToQuit, this, &Application::onAboutToQuit);

    connect(mnuVolume_, &MenuVolume::sigRunMixer, this, &Application::runMixer);
    connect(mnuVolume_, &MenuVolume::sigMuteToggled, this, [this](bool muted) {
        if (!channel_)
            return;

        channel_->setMute(muted);
        updateTrayIcon();
    });
    connect(mnuVolume_, &MenuVolume::sigVolumeChanged, this, &Application::onVolumeChanged);
    connect(trayIcon_, &StatusNotifierItem::activateRequested, this, &Application::onActivateRequested);
    connect(trayIcon_, &StatusNotifierItem::secondaryActivateRequested, this, &Application::onSecondaryActivateRequested);
    connect(trayIcon_, &StatusNotifierItem::scrollRequested, this, &Application::onScrollRequested);
}

void Qtilities::Application::about()
{
    DialogAbout dlg(mnuVolume_);
    centerOnScreen(&dlg);
    dlg.exec();
}

void Qtilities::Application::preferences()
{
    DialogPrefs prefs(mnuVolume_);
    prefs.loadSettings();
    prefs.setDeviceList(deviceList_);

    centerOnScreen(&prefs);

    connect(&prefs, &QDialog::accepted, this, &Application::onPrefsChanged);
    connect(engine_, &AudioEngine::sinkListChanged, this, [&prefs, this] {
        updateDeviceList();
        prefs.setDeviceList(deviceList_);
    });
    prefs.exec();
}

void Qtilities::Application::onPrefsChanged()
{
#if USE_ALSA
    engine_->setNormalized(settings_.isNormalized());
    AlsaEngine* alsa = qobject_cast<AlsaEngine*>(engine_);
    AlsaDevice* dev = qobject_cast<AlsaDevice*>(channel_);
    if (alsa && dev)
        alsa->updateDevice(dev);
#endif
    mnuVolume_->loadSettings();

    onAudioDeviceChanged(settings_.channelId());
    onAudioEngineChanged(settings_.engineId());
}

void Qtilities::Application::onAudioEngineChanged(int engineId)
{
    if (engine_) {
        if (engine_->id() == engineId)
            return;

        if (channel_) {
            disconnect(channel_, nullptr, this, nullptr);
            channel_ = nullptr;
        }
    }
    switch (engineId) {
#if USE_ALSA
    case EngineId::Alsa:
        engine_ = new AlsaEngine(this);
        break;
#endif
#if USE_PULSEAUDIO
    case EngineId::PulseAudio:
        engine_ = new PulseAudioEngine(this);
        break;
#endif
    default:
        engine_ = nullptr;
        return;
    }
#if 0
    engine_->setIgnoreMaxVolume(settings_.ignoreMaxVolume());
#endif
    engine_->setNormalized(settings_.isNormalized());
}

void Qtilities::Application::onAudioDeviceChanged(int deviceId)
{
    if (!engine_ || engine_->sinks().count() <= 0)
        return;

    if (deviceId < 0)
        deviceId = 0;

    channel_ = engine_->sinks().at(deviceId);

    connect(channel_, &AudioDevice::muteChanged, this, [this](bool muted) {
        mnuVolume_->setMute(muted);
        settings_.setMuted(muted);
        updateTrayIcon();
    });
    connect(channel_, &AudioDevice::volumeChanged, this, &Application::onVolumeChanged);
}

void Qtilities::Application::onVolumeChanged(int volume)
{
    if (!channel_)
        return;

    channel_->setVolume(volume);
    settings_.setVolume(volume);
    updateTrayIcon();
}

void Qtilities::Application::onAboutToQuit()
{
    settings_.setUseAutostart(actAutoStart_->isChecked());
    settings_.useAutostart() ? createAutostartFile() : deleteAutostartFile();
    settings_.save();
}

void Qtilities::Application::onActivateRequested(const QPoint&)
{
    if (trayIcon_->status() == StatusNotifierItem::SNIStatus::Active) {
        trayIcon_->setStatus(StatusNotifierItem::SNIStatus::Passive);
        mnuVolume_->hide();
    } else {
        trayIcon_->setStatus(StatusNotifierItem::SNIStatus::Active);
        mnuVolume_->show();
        mnuVolume_->adjustSize();
        mnuVolume_->popUp();
    }
}

void Qtilities::Application::onSecondaryActivateRequested(const QPoint&)
{
    if (channel_ && settings_.muteOnMiddleClick())
        channel_->toggleMute();
}

void Qtilities::Application::onScrollRequested(int delta, Qt::Orientation)
{
    int v = std::clamp(channel_->volume() + delta / 120, 0, 100);
    channel_->setVolume(v);
    mnuVolume_->setVolume(v);
//  trayIcon_->setToolTipTitle(QString("%1\%").arg(v));
    QToolTip::showText(QCursor::pos(), QString("%1\%").arg(v));
    QToolTip::hideText();
}

void Qtilities::Application::runMixer()
{
    QString command = settings_.mixerCommand();
    if (command.isEmpty())
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QStringList args = QProcess::splitCommand(command);
    QProcess::startDetached(args.takeFirst(), args);
#else
    QProcess::startDetached(command);
#endif
}

void Qtilities::Application::updateDeviceList()
{
    if (!engine_)
        return;

    deviceList_.erase(deviceList_.begin(), deviceList_.end());
    for (const AudioDevice *dev : engine_->sinks())
        deviceList_.append(dev->description());
}

void Qtilities::Application::updateTrayIcon()
{
    QString iconName;
    int volume = channel_->volume();
    if (volume <= 0 || channel_->mute())
        iconName = QLatin1String("audio-volume-muted");
    else if (volume <= 33)
        iconName = QLatin1String("audio-volume-low");
    else if (volume <= 66)
        iconName = QLatin1String("audio-volume-medium");
    else
        iconName = QLatin1String("audio-volume-high");

    trayIcon_->setIconByName(iconName);
}

int main(int argc, char* argv[])
{
    // UseHighDpiPixmaps is default from Qt6
#if QT_VERSION < 0x060000
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
    Qtilities::Application app(argc, argv);
    return app.exec();
}
