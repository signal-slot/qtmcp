// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSUBSCRIBEREQUESTPARAMS_H
#define QMCPSUBSCRIBEREQUESTPARAMS_H

#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSubscribeRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpSubscribeRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpSubscribeRequestParams::uri
        \brief The URI of the resource to subscribe to. The URI can use any protocol; it is up to the server how to interpret it.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

public:
    QMcpSubscribeRequestParams() : QMcpGadget(new Private) {}

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

Q_DECLARE_SHARED(QMcpSubscribeRequestParams)

QT_END_NAMESPACE

#endif // QMCPSUBSCRIBEREQUESTPARAMS_H
