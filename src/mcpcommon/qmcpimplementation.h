// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPIMPLEMENTATION_H
#define QMCPIMPLEMENTATION_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpImplementation
    \inmodule QtMcpCommon
    \brief Describes the name and version of an MCP implementation.
*/
class Q_MCPCOMMON_EXPORT QMcpImplementation : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)
    Q_PROPERTY(QString version READ version WRITE setVersion REQUIRED)

public:
    QMcpImplementation() : QMcpGadget(new Private) {}

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    QString version() const {
        return d<Private>()->version;
    }

    void setVersion(const QString &version) {
        if (this->version() == version) return;
        d<Private>()->version = version;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString name;
        QString version;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpImplementation)

QT_END_NAMESPACE

#endif // QMCPIMPLEMENTATION_H
