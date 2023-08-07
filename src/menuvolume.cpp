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
#include "menuvolume.hpp"
#include "application.hpp"
#include "qtilities.hpp"
#include "settings.hpp"

#include <QAction>
#include <QApplication>
#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QMenu>
#include <QScreen>
#include <QSlider>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QDebug>

Qtilities::MenuVolume::MenuVolume(QWidget* parent)
    : QMenu(parent)
    , chkMute_(new QCheckBox(tr("Mute"), this))
    , lblVolume_(new QLabel("0", this))
    , sldVolume_(new QSlider(Qt::Vertical, this))
{
    QWidget* container = new QWidget(this);
    QWidgetAction* actContainer = new QWidgetAction(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QToolButton* tbnMixer = new QToolButton(this);
    QFrame* separator1 = new QFrame(this);
    QFrame* separator2 = new QFrame(this);

    separator1->setFrameShape(QFrame::HLine);
    separator2->setFrameShape(QFrame::HLine);
    separator1->setFrameShadow(QFrame::Sunken);
    separator2->setFrameShadow(QFrame::Sunken);

    tbnMixer->setText(tr("Mixer"));

    lblVolume_->setAlignment(Qt::AlignCenter);

    sldVolume_->setRange(0, 100);
    sldVolume_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sldVolume_->setMinimumHeight(120);
    sldVolume_->setTickPosition(QSlider::TicksBothSides);
    sldVolume_->setTickInterval(10);

    layout->setSizeConstraint(QLayout::SetNoConstraint);
    layout->addWidget(tbnMixer);
    layout->addWidget(separator1);
    layout->addWidget(chkMute_);
    layout->addWidget(separator2);
    layout->addWidget(lblVolume_);
    layout->addWidget(sldVolume_);
    layout->setAlignment(sldVolume_, Qt::AlignHCenter);
    layout->addSpacing(6);

    container->setLayout(layout);
    actContainer->setDefaultWidget(container);
    addAction(actContainer);

    connect(tbnMixer, &QToolButton::released, this, &MenuVolume::sigRunMixer);
    connect(chkMute_, &QCheckBox::clicked, this, &MenuVolume::sigMuteToggled);
    connect(sldVolume_, &QSlider::valueChanged, this, [=](int value) {
        lblVolume_->setText(QString::number(value));
        emit sigVolumeChanged(value);
    });
}

void Qtilities::MenuVolume::loadSettings()
{
    Settings &settings = static_cast<Application *>(qApp)->settings();
    sldVolume_->setPageStep(settings.pageStep());
    sldVolume_->setSingleStep(settings.singleStep());

    int volume = settings.volume();
    if(volume >= 0 && volume <= 100)
        sldVolume_->setValue(volume);
}

void Qtilities::MenuVolume::popUp()
{
    // TODO: move on the appropriated corner depending on the panel position
    QRect rect;
    QPoint pos = QCursor::pos();
    rect.setSize(sizeHint());
    pos.setX(pos.x() - width() / 2);
    rect.moveTopLeft(pos);

    if (const QScreen* screen = screenAt(pos)) {
        const auto& geometry = screen->availableGeometry();

        if (rect.right() > geometry.right())
            rect.moveRight(geometry.right());

        if (rect.bottom() > geometry.bottom())
            rect.moveBottom(geometry.bottom());
    }
    popup(rect.topLeft());
}

void Qtilities::MenuVolume::setMute(bool mute)
{
    chkMute_->blockSignals(true);
    chkMute_->setChecked(mute);
    chkMute_->blockSignals(false);
}

void Qtilities::MenuVolume::setVolume(int volume)
{
    sldVolume_->blockSignals(true);
    sldVolume_->setValue(volume);
    lblVolume_->setText(QString::number(volume));
    sldVolume_->blockSignals(false);
}
