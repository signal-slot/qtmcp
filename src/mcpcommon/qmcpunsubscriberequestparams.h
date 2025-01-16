// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPUNSUBSCRIBEREQUESTPARAMS_H
#define QMCPUNSUBSCRIBEREQUESTPARAMS_H

#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpUnsubscribeRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpUnsubscribeRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpUnsubscribeRequestParams::uri
        \brief The URI of the resource to unsubscribe from.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

public:
    QMcpUnsubscribeRequestParams() : QMcpGadget(new Private) {}

    QUrl uri() const {
        return d<Private>()->uri;
    }

    void setUri(const QUrl& uri) {
        if (this->uri() == uri) return;
        d<Private>()->uri = uri;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QUrl uri;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPUNSUBSCRIBEREQUESTPARAMS_H
