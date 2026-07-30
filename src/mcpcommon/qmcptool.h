// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOL_H
#define QMCPTOOL_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpicon.h>
#include <QtMcpCommon/qmcptoolinputschema.h>
#include <QtMcpCommon/qmcptooloutputschema.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTool
    \inmodule QtMcpCommon
    \brief Definition for a tool the client can call.
*/
class Q_MCPCOMMON_EXPORT QMcpTool : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpTool::description
        \brief A human-readable description of the tool.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpTool::icons
        \brief An optional set of sized icons that the client can display in a user interface.

        \since MCP 2025-11-25
    */
    Q_PROPERTY(QList<QMcpIcon> icons READ icons WRITE setIcons)

    /*!
        \property QMcpTool::inputSchema
        \brief A JSON Schema object defining the expected parameters for the tool.
    */
    Q_PROPERTY(QMcpToolInputSchema inputSchema READ inputSchema WRITE setInputSchema REQUIRED)

    /*!
        \property QMcpTool::name
        \brief The name of the tool.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    /*!
        \property QMcpTool::outputSchema
        \brief An optional JSON Schema object defining the structure of the tool's output.

        The output is returned in the structuredContent property of a
        QMcpCallToolResult.

        \since MCP 2025-06-18
    */
    Q_PROPERTY(QMcpToolOutputSchema outputSchema READ outputSchema WRITE setOutputSchema)

    /*!
        \property QMcpTool::title
        \brief Intended for UI and end-user contexts.

        Optimized to be human-readable and easily understood, even by those
        unfamiliar with domain-specific terminology. If not provided,
        annotations.title should be given precedence over using name.

        \since MCP 2025-06-18
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

public:
    QMcpTool() : QMcpGadget(new Private) {
        qRegisterMetaType<QMcpIcon>();
    }

    QString description() const {
        return d<Private>()->description;
    }

    void setDescription(const QString &description) {
        if (this->description() == description) return;
        d<Private>()->description = description;
    }

    QList<QMcpIcon> icons() const {
        return d<Private>()->icons;
    }

    void setIcons(const QList<QMcpIcon> &icons) {
        if (this->icons() == icons) return;
        d<Private>()->icons = icons;
    }

    QMcpToolInputSchema inputSchema() const {
        return d<Private>()->inputSchema;
    }

    void setInputSchema(const QMcpToolInputSchema &inputSchema) {
        if (this->inputSchema() == inputSchema) return;
        d<Private>()->inputSchema = inputSchema;
    }

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    QMcpToolOutputSchema outputSchema() const {
        return d<Private>()->outputSchema;
    }

    void setOutputSchema(const QMcpToolOutputSchema &outputSchema) {
        if (this->outputSchema() == outputSchema) return;
        d<Private>()->outputSchema = outputSchema;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "icons")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
        if (name == "outputSchema" || name == "title")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QString description;
        QList<QMcpIcon> icons;
        QMcpToolInputSchema inputSchema;
        QString name;
        QMcpToolOutputSchema outputSchema;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTool)

QT_END_NAMESPACE

#endif // QMCPTOOL_H
