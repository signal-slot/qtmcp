// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef TOOLS_H
#define TOOLS_H

#include <QtCore/QObject>
#include <QtGui/QImage>

class Tools : public QObject
{
    Q_OBJECT
public:
    explicit Tools(QObject *parent = nullptr);

    Q_INVOKABLE QImage screenShot() const;
    Q_INVOKABLE void moveCursor(int x, int y);
};

#endif // TOOLS_H
