// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETEREQUESTPARAMS_H
#define QMCPCOMPLETEREQUESTPARAMS_H

#include <QtMcpCommon/qmcpcompleterequestparamsargument.h>
#include <QtMcpCommon/qmcpcompleterequestparamsref.h>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCompleteRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpCompleteRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpCompleteRequestParams::argument
        \brief The argument's information
    */
    Q_PROPERTY(QMcpCompleteRequestParamsArgument argument READ argument WRITE setArgument REQUIRED)
    Q_PROPERTY(QMcpCompleteRequestParamsRef ref READ ref WRITE setRef REQUIRED)

public:
    QMcpCompleteRequestParams() : QMcpGadget(new Private) {}

    QMcpCompleteRequestParamsArgument argument() const {
        return d<Private>()->argument;
    }

    void setArgument(const QMcpCompleteRequestParamsArgument &argument) {
        if (this->argument() == argument) return;
        d<Private>()->argument = argument;
    }

    QMcpCompleteRequestParamsRef ref() const {
        return d<Private>()->ref;
    }

    void setRef(const QMcpCompleteRequestParamsRef &ref) {
        if (this->ref() == ref) return;
        d<Private>()->ref = ref;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpCompleteRequestParamsArgument argument;
        QMcpCompleteRequestParamsRef ref;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCompleteRequestParams)

QT_END_NAMESPACE

#endif // QMCPCOMPLETEREQUESTPARAMS_H
