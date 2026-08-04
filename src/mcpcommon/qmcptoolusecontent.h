// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOLUSECONTENT_H
#define QMCPTOOLUSECONTENT_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpToolUseContent
    \inmodule QtMcpCommon
    \brief A request from the assistant to call a tool.

    Part of the content of a sampling message. The tools the model may request
    are described by the QMcpTool instances passed in
    QMcpCreateMessageRequestParams::tools().

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpToolUseContent : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpToolUseContent::_meta
        \brief Optional metadata about the tool use.

        Clients SHOULD preserve this field when including tool uses in
        subsequent sampling requests to enable caching optimizations.
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    /*!
        \property QMcpToolUseContent::type
        \brief The type of content, always "tool_use".
    */
    Q_PROPERTY(QString type READ type CONSTANT REQUIRED)

    /*!
        \property QMcpToolUseContent::id
        \brief A unique identifier for this tool use.

        This ID is used to match tool results to their corresponding tool uses.
    */
    Q_PROPERTY(QString id READ id WRITE setId REQUIRED)

    /*!
        \property QMcpToolUseContent::input
        \brief The arguments to pass to the tool, conforming to the tool's input schema.
    */
    Q_PROPERTY(QJsonObject input READ input WRITE setInput REQUIRED)

    /*!
        \property QMcpToolUseContent::name
        \brief The name of the tool to call.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

public:
    QMcpToolUseContent() : QMcpGadget(new Private) {}

    QJsonObject meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QJsonObject &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    QString type() const {
        return "tool_use"_L1;
    }

    QString id() const {
        return d<Private>()->id;
    }

    void setId(const QString &id) {
        if (this->id() == id) return;
        d<Private>()->id = id;
    }

    QJsonObject input() const {
        return d<Private>()->input;
    }

    void setInput(const QJsonObject &input) {
        if (this->input() == input) return;
        d<Private>()->input = input;
    }

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject _meta;
        QString id;
        QJsonObject input;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpToolUseContent)

QT_END_NAMESPACE

#endif // QMCPTOOLUSECONTENT_H
