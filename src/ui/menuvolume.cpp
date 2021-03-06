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
#include "src/ui/menuvolume.hpp"

#include <QAction>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidgetAction>

MenuVolume::MenuVolume(QWidget *parent)
    : QMenu(parent)
    , container   (new QWidget    (this))
    , layout      (new QVBoxLayout(container))
    , lblVolume   (new QLabel     (tr("0"),      container))
    , chkMute     (new QCheckBox  (tr("Mute"),   container))
    , pbnMixer    (new QPushButton(tr("Mixer"),  container))
    , sldVolume   (new QSlider    (Qt::Vertical, container))
    , actContainer(new QWidgetAction(this))
{
    lblVolume->setAlignment(Qt::AlignCenter);
    sldVolume->setMaximum(100);
    sldVolume->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    layout->setSizeConstraint(QLayout::SetNoConstraint);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->addWidget(pbnMixer);
    layout->addWidget(chkMute);
    layout->addWidget(lblVolume);
    layout->addWidget(sldVolume);
    container->setLayout(layout);

    actContainer->setDefaultWidget(container);
    addAction(actContainer);
}
QSize MenuVolume::sizeHint() const
{
    return QSize(110, 280);
}
