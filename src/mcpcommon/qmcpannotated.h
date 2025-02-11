// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPANNOTATED_H
#define QMCPANNOTATED_H

#include <QtMcpCommon/qmcpannotations.h>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpAnnotated
    \inmodule QtMcpCommon
    \brief Base for objects that include optional annotations for the client. The client can use annotations to inform how objects are used or displayed
*/
class Q_MCPCOMMON_EXPORT QMcpAnnotated : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)

public:
    QMcpAnnotated() : QMcpGadget(new Private) {}

    QMcpAnnotations annotations() const {
        return d<Private>()->annotations;
    }

    void setAnnotations(const QMcpAnnotations &annotations) {
        if (this->annotations() == annotations) return;
        d<Private>()->annotations = annotations;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpAnnotations annotations;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpAnnotated)

QT_END_NAMESPACE

#endif // QMCPANNOTATED_H
