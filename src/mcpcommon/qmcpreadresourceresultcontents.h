// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPREADRESOURCERESULTCONTENTS_H
#define QMCPREADRESOURCERESULTCONTENTS_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcptextresourcecontents.h>
#include <QtMcpCommon/qmcpblobresourcecontents.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpReadResourceResultContents : public QMcpAnyOf
{
    Q_GADGET

    Q_PROPERTY(QMcpTextResourceContents textResourceContents READ textResourceContents WRITE setTextResourceContents)
    Q_PROPERTY(QMcpBlobResourceContents blobResourceContents READ blobResourceContents WRITE setBlobResourceContents)
public:
    QMcpReadResourceResultContents() : QMcpAnyOf(new Private) {}
    QMcpReadResourceResultContents(const QMcpTextResourceContents &textResourceContents)
        : QMcpAnyOf(new Private) {
        setTextResourceContents(textResourceContents);
    };
    QMcpReadResourceResultContents(const QMcpBlobResourceContents &blobResourceContents)
        : QMcpAnyOf(new Private) {
        setBlobResourceContents(blobResourceContents);
    };

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QMcpTextResourceContents textResourceContents() const {
        return d<Private>()->textResourceContents;
    }

    void setTextResourceContents(const QMcpTextResourceContents &textResourceContents) {
        if (this->textResourceContents() == textResourceContents) return;
        setRefType("textResourceContents"_ba);
        d<Private>()->textResourceContents = textResourceContents;
    }

    QMcpBlobResourceContents blobResourceContents() const {
        return d<Private>()->blobResourceContents;
    }

    void setBlobResourceContents(const QMcpBlobResourceContents &blobResourceContents) {
        if (this->blobResourceContents() == blobResourceContents) return;
        setRefType("blobResourceContents"_ba);
        d<Private>()->blobResourceContents = blobResourceContents;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpTextResourceContents textResourceContents;
        QMcpBlobResourceContents blobResourceContents;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpReadResourceResultContents)

QT_END_NAMESPACE

#endif // QMCPREADRESOURCERESULTCONTENTS_H
