/*
    VolTrayke - Volume tray widget.
    Copyright (C) 2021  Andrea Zanellato <redtid3@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

    SPDX-License-Identifier: GPL-2.0-only
*/
#include "dialogabout.hpp"
#include "ui_dialogabout.h"

#include <QCloseEvent>
#include <QFile>
#include <QIcon>
#include <QPushButton>
#include <QTextStream>

#include <QDebug>

azd::DialogAbout::DialogAbout(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::DialogAbout)
{
    ui->setupUi(this);

    QPixmap pixmap = QIcon::fromTheme("audio-volume-medium").pixmap(48, 48);
    ui->lblIcon->setPixmap(pixmap);

    QFile f(":/about.html");
    f.open(QFile::ReadOnly | QFile::Text);

    QTextStream in(&f);
    QString text = in.readAll();
    f.close();

    ui->txtAbout->setHtml(text);

    setStyleSheet("QTextBrowser { background-color: transparent }");

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &QDialog::hide);
}
azd::DialogAbout::~DialogAbout()
{
    delete ui;
    qDebug() << "Destroyed DialogAbout" << Qt::endl;
}

void azd::DialogAbout::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}
