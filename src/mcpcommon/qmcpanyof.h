// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPANYOF_H
#define QMCPANYOF_H

#include <QtCore/QByteArray>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpAnyOf : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QByteArray refType READ refType WRITE setRefType FINAL)

public:
    QMcpAnyOf() : QMcpGadget(new Private) {}
protected:
    QMcpAnyOf(Private *d) : QMcpGadget(d) {}
public:

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    bool isNull() const {
        return d<Private>()->refType.isEmpty();
    }

    QByteArray refType() const {
        return d<Private>()->refType;
    }

protected:
    void setRefType(const QByteArray &refType) {
        if (this->refType() == refType) return;
        d<Private>()->refType = refType;
    }

public:
    bool fromJsonObject(const QJsonObject &object, const QString &protocolVersion = "2025-03-26"_L1) override;
    QJsonObject toJsonObject(const QString &protocolVersion = "2025-03-26"_L1) const override;

protected:
    struct Private : public QMcpGadget::Private {
        QByteArray refType;
        Private *clone() const override { return new Private(*this); }

        virtual int findPropertyIndex(const QJsonObject &object) const {
            Q_UNUSED(object);
            return -1;
        }
    };
};

Q_DECLARE_SHARED(QMcpAnyOf)

QT_END_NAMESPACE

#endif // QMCPANYOF_H
