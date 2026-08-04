// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOLRESULTCONTENT_H
#define QMCPTOOLRESULTCONTENT_H

#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpcalltoolresultcontent.h>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpToolResultContent
    \inmodule QtMcpCommon
    \brief The result of a tool use, provided by the user back to the assistant.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpToolResultContent : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpToolResultContent::_meta
        \brief Optional metadata about the tool result.

        Clients SHOULD preserve this field when including tool results in
        subsequent sampling requests to enable caching optimizations.
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    /*!
        \property QMcpToolResultContent::type
        \brief The type of content, always "tool_result".
    */
    Q_PROPERTY(QString type READ type CONSTANT REQUIRED)

    /*!
        \property QMcpToolResultContent::toolUseId
        \brief The ID of the tool use this result corresponds to.

        This MUST match the id() of a previous QMcpToolUseContent.
    */
    Q_PROPERTY(QString toolUseId READ toolUseId WRITE setToolUseId REQUIRED)

    /*!
        \property QMcpToolResultContent::content
        \brief The unstructured result content of the tool use.

        This is the same union as QMcpCallToolResult::content(), i.e. text,
        image, audio, resource link and embedded resource.
    */
    Q_PROPERTY(QList<QMcpCallToolResultContent> content READ content WRITE setContent REQUIRED)

    /*!
        \property QMcpToolResultContent::structuredContent
        \brief An optional structured result object.

        If the tool defined an outputSchema, this SHOULD conform to that schema.
    */
    Q_PROPERTY(QJsonObject structuredContent READ structuredContent WRITE setStructuredContent)

    /*!
        \property QMcpToolResultContent::isError
        \brief Whether the tool use resulted in an error.

        If true, the content typically describes the error that occurred.

        The default value is false.
    */
    Q_PROPERTY(bool isError READ isError WRITE setIsError)

public:
    QMcpToolResultContent() : QMcpGadget(new Private) {
        qRegisterMetaType<QMcpCallToolResultContent>();
    }

    QJsonObject meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QJsonObject &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    QString type() const {
        return "tool_result"_L1;
    }

    QString toolUseId() const {
        return d<Private>()->toolUseId;
    }

    void setToolUseId(const QString &toolUseId) {
        if (this->toolUseId() == toolUseId) return;
        d<Private>()->toolUseId = toolUseId;
    }

    QList<QMcpCallToolResultContent> content() const {
        return d<Private>()->content;
    }

    void setContent(const QList<QMcpCallToolResultContent> &content) {
        if (this->content() == content) return;
        d<Private>()->content = content;
    }

    QJsonObject structuredContent() const {
        return d<Private>()->structuredContent;
    }

    void setStructuredContent(const QJsonObject &structuredContent) {
        if (this->structuredContent() == structuredContent) return;
        d<Private>()->structuredContent = structuredContent;
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
    struct Private : public QMcpGadget::Private {
        QJsonObject _meta;
        QString toolUseId;
        QList<QMcpCallToolResultContent> content;
        QJsonObject structuredContent;
        bool isError = false;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpToolResultContent)

QT_END_NAMESPACE

#endif // QMCPTOOLRESULTCONTENT_H
