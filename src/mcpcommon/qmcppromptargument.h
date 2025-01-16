// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPTARGUMENT_H
#define QMCPPROMPTARGUMENT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPromptArgument
    \inmodule QtMcpCommon
    \brief Describes an argument that a prompt can accept.
*/
class Q_MCPCOMMON_EXPORT QMcpPromptArgument : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpPromptArgument::description
        \brief A human-readable description of the argument.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpPromptArgument::name
        \brief The name of the argument.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    /*!
        \property QMcpPromptArgument::required
        \brief Whether this argument must be provided.

        The default value is false.
    */
    Q_PROPERTY(bool required READ required WRITE setRequired)

public:
    QMcpPromptArgument() : QMcpGadget(new Private) {}

    QString description() const {
        return d<Private>()->description;
    }

    void setDescription(const QString &description) {
        if (this->description() == description) return;
        d<Private>()->description = description;
    }

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    bool required() const {
        return d<Private>()->required;
    }

    void setRequired(bool required) {
        if (this->required() == required) return;
        d<Private>()->required = required;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString description;
        QString name;
        bool required = false;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPPROMPTARGUMENT_H
