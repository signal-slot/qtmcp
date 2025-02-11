// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROMPTREFERENCE_H
#define QMCPPROMPTREFERENCE_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpPromptReference
    \inmodule QtMcpCommon
    \brief Identifies a prompt.
*/
class Q_MCPCOMMON_EXPORT QMcpPromptReference : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpPromptReference::name
        \brief The name of the prompt or prompt template
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpPromptReference() : QMcpGadget(new Private) {}

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString& name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    QByteArray type() const {
        return "ref/prompt"_ba;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpPromptReference)

QT_END_NAMESPACE

#endif // QMCPPROMPTREFERENCE_H
