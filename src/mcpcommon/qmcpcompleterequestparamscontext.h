// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETEREQUESTPARAMSCONTEXT_H
#define QMCPCOMPLETEREQUESTPARAMSCONTEXT_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCompleteRequestParamsContext
    \inmodule QtMcpCommon
    \brief Additional, optional context for completions.
    \since MCP 2025-06-18
*/
class Q_MCPCOMMON_EXPORT QMcpCompleteRequestParamsContext : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpCompleteRequestParamsContext::arguments
        \brief Previously-resolved variables in a URI template or prompt.
    */
    Q_PROPERTY(QJsonObject arguments READ arguments WRITE setArguments)

public:
    QMcpCompleteRequestParamsContext() : QMcpGadget(new Private) {}

    QJsonObject arguments() const {
        return d<Private>()->arguments;
    }

    void setArguments(const QJsonObject &arguments) {
        if (this->arguments() == arguments) return;
        d<Private>()->arguments = arguments;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject arguments;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCompleteRequestParamsContext)

QT_END_NAMESPACE

#endif // QMCPCOMPLETEREQUESTPARAMSCONTEXT_H
