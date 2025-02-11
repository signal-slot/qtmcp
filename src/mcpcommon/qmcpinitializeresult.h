// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPINITIALIZERESULT_H
#define QMCPINITIALIZERESULT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>
#include <QtMcpCommon/qmcpimplementation.h>
#include <QtMcpCommon/qmcpservercapabilities.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpInitializeResult
    \inmodule QtMcpCommon
    \brief After receiving an initialize request from the client, the server sends this response.
*/
class Q_MCPCOMMON_EXPORT QMcpInitializeResult : public QMcpResult
{
    Q_GADGET

    Q_PROPERTY(QMcpServerCapabilities capabilities READ capabilities WRITE setCapabilities REQUIRED)

    /*!
        \property QMcpInitializeResult::instructions
        \brief Instructions describing how to use the server and its features.
        
        This can be used by clients to improve the LLM's understanding of available tools, resources, etc. It can be thought of like a "hint" to the model. For example, this information MAY be added to the system prompt.
    */
    Q_PROPERTY(QString instructions READ instructions WRITE setInstructions)

    /*!
        \property QMcpInitializeResult::protocolVersion
        \brief The version of the Model Context Protocol that the server wants to use. This may not match the version that the client requested. If the client cannot support this version, it MUST disconnect.
    */
    Q_PROPERTY(QString protocolVersion READ protocolVersion WRITE setProtocolVersion REQUIRED)

    Q_PROPERTY(QMcpImplementation serverInfo READ serverInfo WRITE setServerInfo REQUIRED)

public:
    QMcpInitializeResult() : QMcpResult(new Private) {}

    QMcpServerCapabilities capabilities() const {
        return d<Private>()->capabilities;
    }

    void setCapabilities(const QMcpServerCapabilities &value) {
        if (capabilities() == value) return;
        d<Private>()->capabilities = value;
    }

    QString instructions() const {
        return d<Private>()->instructions;
    }

    void setInstructions(const QString &value) {
        if (instructions() == value) return;
        d<Private>()->instructions = value;
    }

    QString protocolVersion() const {
        return d<Private>()->protocolVersion;
    }

    void setProtocolVersion(const QString &value) {
        if (protocolVersion() == value) return;
        d<Private>()->protocolVersion = value;
    }

    QMcpImplementation serverInfo() const {
        return d<Private>()->serverInfo;
    }

    void setServerInfo(const QMcpImplementation &value) {
        if (serverInfo() == value) return;
        d<Private>()->serverInfo = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QMcpServerCapabilities capabilities;
        QString instructions;
        QString protocolVersion;
        QMcpImplementation serverInfo;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpInitializeResult)

QT_END_NAMESPACE

#endif // QMCPINITIALIZERESULT_H
