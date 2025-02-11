// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOLLISTCHANGEDNOTIFICATIONPARAMS_H
#define QMCPTOOLLISTCHANGEDNOTIFICATIONPARAMS_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcptoollistchangednotificationparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpToolListChangedNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpToolListChangedNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpToolListChangedNotificationParams::_meta
        \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
    */
    Q_PROPERTY(QMcpToolListChangedNotificationParamsMeta _meta READ meta WRITE setMeta)

public:
    QMcpToolListChangedNotificationParams() : QMcpNotificationParams(new Private) {}

    QMcpToolListChangedNotificationParamsMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpToolListChangedNotificationParamsMeta &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QMcpToolListChangedNotificationParamsMeta _meta;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpToolListChangedNotificationParams)

QT_END_NAMESPACE

#endif // QMCPTOOLLISTCHANGEDNOTIFICATIONPARAMS_H
