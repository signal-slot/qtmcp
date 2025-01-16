// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCNOTIFICATION_H
#define QMCPJSONRPCNOTIFICATION_H

#include <QtMcpCommon/qmcpjsonrpcmessage.h>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpjsonrpcnotificationparams.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCNotification
    \inmodule QtMcpCommon
    \brief A notification which does not expect a response.
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCNotification : public QMcpJSONRPCMessage
{
    Q_GADGET

    Q_PROPERTY(QString method READ method CONSTANT REQUIRED)
    Q_PROPERTY(QMcpJSONRPCNotificationParams params READ params WRITE setParams)

public:
    QMcpJSONRPCNotification() : QMcpJSONRPCMessage(new Private) {}
protected:
    QMcpJSONRPCNotification(Private *d) : QMcpJSONRPCMessage(d) {}
public:
    virtual QString method() const = 0;

    QMcpJSONRPCNotificationParams params() const {
        return d<Private>()->params;
    }

    void setParams(const QMcpJSONRPCNotificationParams &params) {
        if (this->params() == params) return;
        d<Private>()->params = params;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    struct Private : public QMcpJSONRPCMessage::Private {
        QString method;
        QMcpJSONRPCNotificationParams params;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPJSONRPCNOTIFICATION_H
