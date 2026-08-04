// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETEREQUESTPARAMS_H
#define QMCPCOMPLETEREQUESTPARAMS_H

#include <QtMcpCommon/qmcpcompleterequestparamsargument.h>
#include <QtMcpCommon/qmcpcompleterequestparamscontext.h>
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
    /*!
        \property QMcpCompleteRequestParams::context
        \brief Additional, optional context for completions.
        \since MCP 2025-06-18
    */
    Q_PROPERTY(QMcpCompleteRequestParamsContext context READ context WRITE setContext)
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

    QMcpCompleteRequestParamsContext context() const {
        return d<Private>()->context;
    }

    void setContext(const QMcpCompleteRequestParamsContext &context) {
        if (this->context() == context) return;
        d<Private>()->context = context;
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

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "context")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpCompleteRequestParamsArgument argument;
        QMcpCompleteRequestParamsContext context;
        QMcpCompleteRequestParamsRef ref;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCompleteRequestParams)

QT_END_NAMESPACE

#endif // QMCPCOMPLETEREQUESTPARAMS_H
