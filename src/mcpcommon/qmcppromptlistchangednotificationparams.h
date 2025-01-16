// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPTLISTCHANGEDNOTIFICATIONPARAMS_H
#define QMCPPROMPTLISTCHANGEDNOTIFICATIONPARAMS_H

#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcppromptlistchangednotificationparamsmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPromptListChangedNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpPromptListChangedNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpPromptListChangedNotificationParams::_meta
        \brief This parameter name is reserved by MCP to allow clients and servers to attach additional metadata to their notifications.
    */
    Q_PROPERTY(QMcpPromptListChangedNotificationParamsMeta _meta READ _meta WRITE setMeta)

public:
    QMcpPromptListChangedNotificationParams() : QMcpNotificationParams(new Private) {}

    QMcpPromptListChangedNotificationParamsMeta _meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpPromptListChangedNotificationParamsMeta &_meta) {
        if (this->_meta() == _meta) return;
        d<Private>()->_meta = _meta;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QMcpPromptListChangedNotificationParamsMeta _meta;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPPROMPTLISTCHANGEDNOTIFICATIONPARAMS_H
