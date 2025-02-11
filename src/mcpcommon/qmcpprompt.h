// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPT_H
#define QMCPPROMPT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcppromptargument.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpPrompt
    \inmodule QtMcpCommon
    \brief A prompt or prompt template that the server offers.
*/
class Q_MCPCOMMON_EXPORT QMcpPrompt : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpPrompt::arguments
        \brief A list of arguments to use for templating the prompt.
    */
    Q_PROPERTY(QList<QMcpPromptArgument> arguments READ arguments WRITE setArguments)

    /*!
        \property QMcpPrompt::description
        \brief An optional description of what this prompt provides
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpPrompt::name
        \brief The name of the prompt or prompt template.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

public:
    QMcpPrompt() : QMcpGadget(new Private) {
        qRegisterMetaType<QMcpPromptArgument>();
    }

    QList<QMcpPromptArgument> arguments() const {
        return d<Private>()->arguments;
    }

    void setArguments(const QList<QMcpPromptArgument> &arguments) {
        if (this->arguments() == arguments) return;
        d<Private>()->arguments = arguments;
    }

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

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QList<QMcpPromptArgument> arguments;
        QString description;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPrompt)

QT_END_NAMESPACE

#endif // QMCPPROMPT_H
