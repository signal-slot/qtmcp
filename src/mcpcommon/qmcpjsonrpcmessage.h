// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCMESSAGE_H
#define QMCPJSONRPCMESSAGE_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcprequestid.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpJSONRPCMessage : public QMcpGadget
{
    Q_GADGET
    Q_PROPERTY(QString jsonrpc READ jsonrpc CONSTANT REQUIRED)

public:
    QMcpJSONRPCMessage() : QMcpJSONRPCMessage(new Private) {}
protected:
    QMcpJSONRPCMessage(QMcpJSONRPCMessage::Private *d) : QMcpGadget(d) {}
public:
    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QString jsonrpc() const {
        return "2.0"_L1;
    }

protected:
    struct Private : public QMcpGadget::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

class Q_MCPCOMMON_EXPORT QMcpJSONRPCMessageWithId : public QMcpJSONRPCMessage
{
    Q_GADGET
    Q_PROPERTY(QMcpRequestId id READ id WRITE setId REQUIRED)

public:
    QMcpJSONRPCMessageWithId() : QMcpJSONRPCMessage(new Private) {}
protected:
    QMcpJSONRPCMessageWithId(QMcpJSONRPCMessageWithId::Private *d) : QMcpJSONRPCMessage(d) {}
public:
    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QMcpRequestId id() const {
        return d<Private>()->id;
    }

    void setId(const QMcpRequestId &id) {
        if (this->id() == id) return;
        d<Private>()->id = id;
    }

protected:
    struct Private : public QMcpJSONRPCMessage::Private {
        QMcpRequestId id;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPJSONRPCMESSAGE_H
