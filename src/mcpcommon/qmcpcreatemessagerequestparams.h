// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCREATEMESSAGEREQUESTPARAMS_H
#define QMCPCREATEMESSAGEREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtMcpCommon/qmcpcreatemessagerequestparamsmetadata.h>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpmodelpreferences.h>
#include <QtMcpCommon/qmcpsamplingmessage.h>
#include <QtMcpCommon/qmcptool.h>
#include <QtMcpCommon/qmcptoolchoice.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpCreateMessageRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpCreateMessageRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpCreateMessageRequestParams::includeContext
        \brief A request to include context from one or more MCP servers (including the caller), to be attached to the prompt. The client MAY ignore this request.
    */
    Q_PROPERTY(QString includeContext READ includeContext WRITE setIncludeContext)

    /*!
        \property QMcpCreateMessageRequestParams::maxTokens
        \brief The maximum number of tokens to sample, as requested by the server. The client MAY choose to sample fewer tokens than requested.

        The default value is 0.
    */
    Q_PROPERTY(int maxTokens READ maxTokens WRITE setMaxTokens REQUIRED)

    Q_PROPERTY(QList<QMcpSamplingMessage> messages READ messages WRITE setMessages REQUIRED)

    /*!
        \property QMcpCreateMessageRequestParams::metadata
        \brief Optional metadata to pass through to the LLM provider. The format of this metadata is provider-specific.
    */
    Q_PROPERTY(QMcpCreateMessageRequestParamsMetadata metadata READ metadata WRITE setMetadata)

    /*!
        \property QMcpCreateMessageRequestParams::modelPreferences
        \brief The server's preferences for which model to select. The client MAY ignore these preferences.
    */
    Q_PROPERTY(QMcpModelPreferences modelPreferences READ modelPreferences WRITE setModelPreferences)

    Q_PROPERTY(QList<QString> stopSequences READ stopSequences WRITE setStopSequences)

    /*!
        \property QMcpCreateMessageRequestParams::systemPrompt
        \brief An optional system prompt the server wants to use for sampling. The client MAY modify or omit this prompt.
    */
    Q_PROPERTY(QString systemPrompt READ systemPrompt WRITE setSystemPrompt)

    Q_PROPERTY(qreal temperature READ temperature WRITE setTemperature)

    /*!
        \property QMcpCreateMessageRequestParams::toolChoice
        \brief Controls how the model uses tools.

        The client MUST return an error if this field is provided but
        QMcpClientCapabilitiesSampling does not declare "tools". The default is
        \c {{ mode: "auto" }}.

        \since MCP 2025-11-25
    */
    Q_PROPERTY(QMcpToolChoice toolChoice READ toolChoice WRITE setToolChoice)

    /*!
        \property QMcpCreateMessageRequestParams::tools
        \brief Tools that the model may use during generation.

        The client MUST return an error if this field is provided but
        QMcpClientCapabilitiesSampling does not declare "tools".

        \since MCP 2025-11-25
    */
    Q_PROPERTY(QList<QMcpTool> tools READ tools WRITE setTools)

public:
    QMcpCreateMessageRequestParams() : QMcpGadget(new Private) {
        qRegisterMetaType<QMcpSamplingMessage>();
        qRegisterMetaType<QMcpTool>();
    }

    QString includeContext() const { return d<Private>()->includeContext; }
    void setIncludeContext(const QString &value) {
        if (includeContext() == value) return;
        d<Private>()->includeContext = value;
    }

    int maxTokens() const { return d<Private>()->maxTokens; }
    void setMaxTokens(int value) {
        if (maxTokens() == value) return;
        d<Private>()->maxTokens = value;
    }

    QList<QMcpSamplingMessage> messages() const { return d<Private>()->messages; }
    void setMessages(const QList<QMcpSamplingMessage> &value) {
        if (messages() == value) return;
        d<Private>()->messages = value;
    }

    QMcpCreateMessageRequestParamsMetadata metadata() const { return d<Private>()->metadata; }
    void setMetadata(const QMcpCreateMessageRequestParamsMetadata &value) {
        if (metadata() == value) return;
        d<Private>()->metadata = value;
    }

    QMcpModelPreferences modelPreferences() const { return d<Private>()->modelPreferences; }
    void setModelPreferences(const QMcpModelPreferences &value) {
        if (modelPreferences() == value) return;
        d<Private>()->modelPreferences = value;
    }

    QList<QString> stopSequences() const { return d<Private>()->stopSequences; }
    void setStopSequences(const QList<QString> &value) {
        if (stopSequences() == value) return;
        d<Private>()->stopSequences = value;
    }

    QString systemPrompt() const { return d<Private>()->systemPrompt; }
    void setSystemPrompt(const QString &value) {
        if (systemPrompt() == value) return;
        d<Private>()->systemPrompt = value;
    }

    qreal temperature() const { return d<Private>()->temperature; }
    void setTemperature(qreal value) {
        if (temperature() == value) return;
        d<Private>()->temperature = value;
    }

    QMcpToolChoice toolChoice() const { return d<Private>()->toolChoice; }
    void setToolChoice(const QMcpToolChoice &value) {
        if (toolChoice() == value) return;
        d<Private>()->toolChoice = value;
    }

    QList<QMcpTool> tools() const { return d<Private>()->tools; }
    void setTools(const QList<QMcpTool> &value) {
        if (tools() == value) return;
        d<Private>()->tools = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "toolChoice" || name == "tools")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QString includeContext;
        int maxTokens = 0;
        QList<QMcpSamplingMessage> messages;
        QMcpCreateMessageRequestParamsMetadata metadata;
        QMcpModelPreferences modelPreferences;
        QList<QString> stopSequences;
        QString systemPrompt;
        qreal temperature = 0;
        QMcpToolChoice toolChoice;
        QList<QMcpTool> tools;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCreateMessageRequestParams)

QT_END_NAMESPACE

#endif // QMCPCREATEMESSAGEREQUESTPARAMS_H
