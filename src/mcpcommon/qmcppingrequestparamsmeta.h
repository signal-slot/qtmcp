// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPINGREQUESTPARAMSMETA_H
#define QMCPPINGREQUESTPARAMSMETA_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpprogresstoken.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPingRequestParamsMeta
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpPingRequestParamsMeta : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpPingRequestParamsMeta::progressToken
        \brief If specified, the caller is requesting out-of-band progress notifications for this request (as represented by notifications/progress). The value of this parameter is an opaque token that will be attached to any subsequent notifications. The receiver is not obligated to provide these notifications.
    */
    Q_PROPERTY(QMcpProgressToken progressToken READ progressToken WRITE setProgressToken)

public:
    QMcpPingRequestParamsMeta() : QMcpGadget(new Private) {}

    QMcpProgressToken progressToken() const {
        return d<Private>()->progressToken;
    }

    void setProgressToken(const QMcpProgressToken &token) {
        if (this->progressToken() == token) return;
        d<Private>()->progressToken = token;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpProgressToken progressToken;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPingRequestParamsMeta)

QT_END_NAMESPACE

#endif // QMCPPINGREQUESTPARAMSMETA_H
