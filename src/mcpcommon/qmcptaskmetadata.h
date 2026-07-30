// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTASKMETADATA_H
#define QMCPTASKMETADATA_H

#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTaskMetadata
    \inmodule QtMcpCommon
    \brief Metadata for augmenting a request with task execution.

    Include this in the \c task field of the request parameters.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpTaskMetadata : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpTaskMetadata::ttl
        \brief Requested duration in milliseconds to retain task from creation.

        The default value is 0, in which case the member is omitted.
    */
    Q_PROPERTY(int ttl READ ttl WRITE setTtl)

public:
    QMcpTaskMetadata() : QMcpGadget(new Private) {}

    int ttl() const {
        return d<Private>()->ttl;
    }

    void setTtl(int ttl) {
        if (this->ttl() == ttl) return;
        d<Private>()->ttl = ttl;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        int ttl = 0;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTaskMetadata)

QT_END_NAMESPACE

#endif // QMCPTASKMETADATA_H
