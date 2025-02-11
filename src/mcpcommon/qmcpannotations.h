// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPANNOTATIONS_H
#define QMCPANNOTATIONS_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcprole.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpAnnotations
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpAnnotations : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpAnnotations::audience
        \brief Describes who the intended customer of this object or data is.

        It can include multiple entries to indicate content useful for multiple audiences (e.g., `["user", "assistant"]`).
    */
    Q_PROPERTY(QList<QMcpRole::QMcpRole> audience READ audience WRITE setAudience)

    /*!
        \property QMcpAnnotations::priority
        \brief Describes how important this data is for operating the server.

        A value of 1 means "most important," and indicates that the data is
        effectively required, while 0 means "least important," and indicates that
        the data is entirely optional.

        The default value is 0.
    */
    Q_PROPERTY(qreal priority READ priority WRITE setPriority)

public:
    QMcpAnnotations() : QMcpGadget(new Private) {}

    QList<QMcpRole::QMcpRole> audience() const {
        return d<Private>()->audience;
    }

    void setAudience(const QList<QMcpRole::QMcpRole> &audience) {
        if (this->audience() == audience) return;
        d<Private>()->audience = audience;
    }

    qreal priority() const {
        return d<Private>()->priority;
    }

    void setPriority(qreal priority) {
        if (this->priority() == priority) return;
        d<Private>()->priority = priority;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QList<QMcpRole::QMcpRole> audience;
        qreal priority = 0;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpAnnotations)

QT_END_NAMESPACE

#endif // QMCPANNOTATIONS_H
