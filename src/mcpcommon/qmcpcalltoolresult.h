// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCALLTOOLRESULT_H
#define QMCPCALLTOOLRESULT_H

#include <QtMcpCommon/qmcpcalltoolresultcontent.h>
#include <QtCore/QList>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCallToolResult
    \inmodule QtMcpCommon
    \brief The server's response to a tool call.
    
    Any errors that originate from the tool SHOULD be reported inside the result
    object, with `isError` set to true, _not_ as an MCP protocol-level error
    response. Otherwise, the LLM would not be able to see that an error occurred
    and self-correct.
    
    However, any errors in _finding_ the tool, an error indicating that the
    server does not support tool calls, or any other exceptional conditions,
    should be reported as an MCP error response.
*/
class Q_MCPCOMMON_EXPORT QMcpCallToolResult : public QMcpResult
{
    Q_GADGET

    Q_PROPERTY(QList<QMcpCallToolResultContent> content READ content WRITE setContent REQUIRED)
    /*!
        \property QMcpCallToolResult::isError
        \brief Whether the tool call ended in an error.
        
        If not set, this is assumed to be false (the call was successful).

        The default value is false.
    */
    Q_PROPERTY(bool isError READ isError WRITE setIsError)

public:
    QMcpCallToolResult() : QMcpResult(new Private) {
        qRegisterMetaType<QMcpCallToolResultContent>();
    }

    QList<QMcpCallToolResultContent> content() const {
        return d<Private>()->content;
    }

    void setContent(const QList<QMcpCallToolResultContent> &content) {
        if (this->content() == content) return;
        d<Private>()->content = content;
    }

    bool isError() const {
        return d<Private>()->isError;
    }

    void setIsError(bool isError) {
        if (this->isError() == isError) return;
        d<Private>()->isError = isError;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QList<QMcpCallToolResultContent> content;
        bool isError = false;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCallToolResult)

QT_END_NAMESPACE

#endif // QMCPCALLTOOLRESULT_H
