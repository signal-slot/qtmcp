// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERRESULT_H
#define QMCPSERVERRESULT_H

#include <QtMcpCommon/qmcpcommonglobal.h>
#include <QtMcpCommon/qmcpanyof.h>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpinitializeresult.h>
#include <QtMcpCommon/qmcplistresourcesresult.h>
#include <QtMcpCommon/qmcplistresourcetemplatesresult.h>
#include <QtMcpCommon/qmcpreadresourceresult.h>
#include <QtMcpCommon/qmcplistpromptsresult.h>
#include <QtMcpCommon/qmcpgetpromptresult.h>
#include <QtMcpCommon/qmcplisttoolsresult.h>
#include <QtMcpCommon/qmcpcalltoolresult.h>
#include <QtMcpCommon/qmcpcompleteresult.h>

QT_BEGIN_NAMESPACE

class Q_MCPCOMMON_EXPORT QMcpServerResult : public QMcpAnyOf
{
    Q_GADGET
public:
    QMcpServerResult() : QMcpAnyOf(new Private) {}

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpAnyOf::Private {
        QMcpResult result;
        QMcpInitializeResult initializeResult;
        QMcpListResourcesResult listResourcesResult;
        QMcpListResourceTemplatesResult listResourceTemplatesResult;
        QMcpReadResourceResult readResourceResult;
        QMcpListPromptsResult listPromptsResult;
        QMcpGetPromptResult getPromptResult;
        QMcpListToolsResult listToolsResult;
        QMcpCallToolResult callToolResult;
        QMcpCompleteResult completeResult;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPSERVERRESULT_H
