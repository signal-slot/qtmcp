// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#include "tools.h"
#include <QtGui/QCursor>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

Tools::Tools(QObject *parent)
    : QObject(parent)
{}

QImage Tools::screenShot() const
{
    return QGuiApplication::primaryScreen()->grabWindow().toImage();
}

void Tools::moveCursor(int x, int y)
{
    QCursor::setPos(x, y);
}
