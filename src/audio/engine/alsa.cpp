/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Johannes Zellner <webmaster@nebulon.de>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "audio/engine/alsa.hpp"
#include "audio/device/alsa.hpp"

#include <QMetaType>
#include <QSocketNotifier>
#include <QtDebug>

#include <cmath>

AlsaEngine* AlsaEngine::m_instance = nullptr;

static int alsa_elem_event_callback(snd_mixer_elem_t* elem, unsigned int /*mask*/)
{
    AlsaEngine* engine = AlsaEngine::instance();
    if (engine)
        engine->updateDevice(engine->getDeviceByAlsaElem(elem));

    return 0;
}

static int alsa_mixer_event_callback(snd_mixer_t* /*mixer*/, unsigned int /*mask*/, snd_mixer_elem_t* /*elem*/)
{
    return 0;
}

AlsaEngine::AlsaEngine(QObject* parent)
    : AudioEngine(parent)
{
    discoverDevices();
    m_instance = this;
}

AlsaEngine* AlsaEngine::instance()
{
    return m_instance;
}

int AlsaEngine::volumeMax(AudioDevice* device) const
{
    AlsaDevice* alsa_dev = qobject_cast<AlsaDevice*>(device);
    Q_ASSERT(alsa_dev);
    return alsa_dev->volumeMax();
}

AlsaDevice* AlsaEngine::getDeviceByAlsaElem(snd_mixer_elem_t* elem) const
{
    for (AudioDevice* device : qAsConst(m_sinks)) {
        AlsaDevice* dev = qobject_cast<AlsaDevice*>(device);
        if (!dev || !dev->element())
            continue;

        if (dev->element() == elem)
            return dev;
    }

    return nullptr;
}

void AlsaEngine::commitDeviceVolume(AudioDevice* device)
{
    AlsaDevice* dev = qobject_cast<AlsaDevice*>(device);
    snd_mixer_elem_t* elem = dev->element();

    if (!dev || !elem)
        return;

    // See https://github.com/alsa-project/alsa-utils/blob/master/alsamixer/volume_mapping.c#L120
    double volume = static_cast<double>(dev->volume()) / 100.0;
    long min, max, val;

    if (m_isNormalized) {
        snd_mixer_selem_get_playback_dB_range(elem, &min, &max);

        if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
            double minNorm = pow(10, (min - max) / 6000.0);
            volume = volume * (1 - minNorm) + minNorm;
        }
        val = lrint(6000.0 * log10(volume)) + max;
        snd_mixer_selem_set_playback_dB_all(elem, val, 0);
    } else {
        min = dev->volumeMin();
        max = dev->volumeMax();
        val = lrint(volume * (max - min)) + min;
        snd_mixer_selem_set_playback_volume_all(elem, val);
    }
    qDebug() << "value: " << val;
    qDebug() << "volume: " << volume;
}

void AlsaEngine::setMute(AudioDevice* device, bool state)
{
    AlsaDevice* dev = qobject_cast<AlsaDevice*>(device);
    if (!dev || !dev->element())
        return;

    if (snd_mixer_selem_has_playback_switch(dev->element()))
        snd_mixer_selem_set_playback_switch_all(dev->element(), (int)!state);
    else if (state)
        dev->setVolume(0);
}

void AlsaEngine::updateDevice(AlsaDevice* device)
{
    if (!device)
        return;

    // See https://github.com/alsa-project/alsa-utils/blob/master/alsamixer/volume_mapping.c#L83
    snd_mixer_selem_channel_id_t channel = static_cast<snd_mixer_selem_channel_id_t>(0);
    snd_mixer_elem_t* elem = device->element();
    long min, max, value;
    double volume;

    if (m_isNormalized) {
        snd_mixer_selem_get_playback_dB(elem, channel, &value);
        snd_mixer_selem_get_playback_dB_range(elem, &min, &max);
        volume = pow(10, (value - max) / 6000.0) * 100.0;

        if (min != SND_CTL_TLV_DB_GAIN_MUTE) {
            double minNorm = pow(10, (min - max) / 6000.0);
            volume = (volume - minNorm) / (1 - minNorm);
        }
        device->setVolumeNoCommit(volume);
    } else {
        min = device->volumeMin();
        max = device->volumeMax();

        snd_mixer_selem_get_playback_volume(elem, channel, &value);
        volume = lrint((static_cast<double>(value - min) * 100.0) / (max - min));
        device->setVolumeNoCommit(volume);
    }
    qDebug() << "value:" << value;
    qDebug() << "volume:" << volume;
#if 0
    qDebug() << "updateDevice:" << device->name() << value;
#endif
    if (snd_mixer_selem_has_playback_switch(elem)) {
        int mute;
        snd_mixer_selem_get_playback_switch(elem, channel, &mute);
        device->setMuteNoCommit(!(bool)mute);
    }
}

void AlsaEngine::driveAlsaEventHandling(int fd)
{
    snd_mixer_handle_events(m_mixerMap.value(fd));
}

void AlsaEngine::discoverDevices()
{
    int error;
    int cardNum = -1;
    const int BUFF_SIZE = 64;

    while (true) {
        if ((error = snd_card_next(&cardNum)) < 0) {
            qWarning("Can't get the next card number: %s\n", snd_strerror(error));
            break;
        }

        if (cardNum < 0)
            break;

        char str[BUFF_SIZE];
        const size_t n = snprintf(str, sizeof(str), "hw:%i", cardNum);
        if (BUFF_SIZE <= n) {
            qWarning("AlsaEngine::discoverDevices: Buffer too small\n");
            continue;
        }

        snd_ctl_t* cardHandle;
        if ((error = snd_ctl_open(&cardHandle, str, 0)) < 0) {
            qWarning("Can't open card %i: %s\n", cardNum, snd_strerror(error));
            continue;
        }

        snd_ctl_card_info_t* cardInfo;
        snd_ctl_card_info_alloca(&cardInfo);

        QString cardName = QString::fromLatin1(snd_ctl_card_info_get_name(cardInfo));
        if (cardName.isEmpty())
            cardName = QString::fromLatin1(str);

        if ((error = snd_ctl_card_info(cardHandle, cardInfo)) < 0) {
            qWarning("Can't get info for card %i: %s\n", cardNum, snd_strerror(error));
        } else {
            // setup mixer and iterate over channels
            snd_mixer_t* mixer = nullptr;
            snd_mixer_open(&mixer, 0);
            snd_mixer_attach(mixer, str);
            snd_mixer_selem_register(mixer, nullptr, nullptr);
            snd_mixer_load(mixer);

            // setup event handler for mixer
            snd_mixer_set_callback(mixer, alsa_mixer_event_callback);

            // setup eventloop handling
            struct pollfd pfd;
            if (snd_mixer_poll_descriptors(mixer, &pfd, 1)) {
                QSocketNotifier* notifier = new QSocketNotifier(pfd.fd, QSocketNotifier::Read, this);
                connect(notifier, &QSocketNotifier::activated, this, [this](QSocketDescriptor socket, QSocketNotifier::Type) { this->driveAlsaEventHandling(socket); });
                m_mixerMap.insert(pfd.fd, mixer);
            }

            snd_mixer_elem_t* mixerElem = nullptr;
            mixerElem = snd_mixer_first_elem(mixer);

            while (mixerElem) {
                // check if we have a Sink or Source
                if (snd_mixer_selem_has_playback_volume(mixerElem)) {
                    AlsaDevice* dev = new AlsaDevice(Sink, this, this);
                    dev->setName(QString::fromLatin1(snd_mixer_selem_get_name(mixerElem)));
                    dev->setIndex(cardNum);
                    dev->setDescription(cardName + QStringLiteral(" - ") + dev->name());

                    // set alsa specific members
                    dev->setCardName(QString::fromLatin1(str));
                    dev->setMixer(mixer);
                    dev->setElement(mixerElem);

                    // get & store the range
                    long min, max;
                    snd_mixer_selem_get_playback_volume_range(mixerElem, &min, &max);
                    dev->setVolumeMinMax(min, max);

                    updateDevice(dev);

                    // register event callback
                    snd_mixer_elem_set_callback(mixerElem, alsa_elem_event_callback);

                    m_sinks.append(dev);
                }

                mixerElem = snd_mixer_elem_next(mixerElem);
            }
        }

        snd_ctl_close(cardHandle);
    }

    snd_config_update_free_global();
}

void AlsaEngine::setNormalized(bool normalized)
{
    m_isNormalized = normalized;
}
