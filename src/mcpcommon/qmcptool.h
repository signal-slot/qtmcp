// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOL_H
#define QMCPTOOL_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcptoolinputschema.h>

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
        \property QMcpTool::inputSchema
        \brief A JSON Schema object defining the expected parameters for the tool.
    */
    Q_PROPERTY(QMcpToolInputSchema inputSchema READ inputSchema WRITE setInputSchema REQUIRED)

    /*!
        \property QMcpTool::name
        \brief The name of the tool.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

public:
    QMcpTool() : QMcpGadget(new Private) {}

    QString description() const {
        return d<Private>()->description;
    }

    void setDescription(const QString &description) {
        if (this->description() == description) return;
        d<Private>()->description = description;
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

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString description;
        QMcpToolInputSchema inputSchema;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTool)

QT_END_NAMESPACE

#endif // QMCPTOOL_H
