// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPJSONRPCERRORERROR_H
#define QMCPJSONRPCERRORERROR_H

#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpJSONRPCErrorError
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpJSONRPCErrorError : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpJSONRPCErrorError::code
        \brief The error type that occurred.

        The default value is 0.
    */
    Q_PROPERTY(int code READ code WRITE setCode REQUIRED)

    /*!
        \property QMcpJSONRPCErrorError::data
        \brief Additional information about the error. The value of this member is defined by the sender (e.g. detailed error information, nested errors etc.).
    */
    Q_PROPERTY(QJsonValue data READ data WRITE setData)

    /*!
        \property QMcpJSONRPCErrorError::message
        \brief A short description of the error. The message SHOULD be limited to a concise single sentence.
    */
    Q_PROPERTY(QString message READ message WRITE setMessage REQUIRED)

public:
    QMcpJSONRPCErrorError() : QMcpGadget(new Private) {}

    int code() const {
        return d<Private>()->code;
    }

    void setCode(int value) {
        if (code() == value) return;
        d<Private>()->code = value;
    }

    QJsonValue data() const {
        return d<Private>()->data;
    }

    void setData(const QJsonValue &value) {
        if (data() == value) return;
        d<Private>()->data = value;
    }

    QString message() const {
        return d<Private>()->message;
    }

    void setMessage(const QString &value) {
        if (message() == value) return;
        d<Private>()->message = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        int code = 0;
        QJsonValue data;
        QString message;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpJSONRPCErrorError)

QT_END_NAMESPACE

#endif // QMCPJSONRPCERRORERROR_H
