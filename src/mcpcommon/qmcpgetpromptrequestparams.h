// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPGETPROMPTREQUESTPARAMS_H
#define QMCPGETPROMPTREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpGetPromptRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpGetPromptRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpGetPromptRequestParams::arguments
        \brief Arguments to use for templating the prompt.
    */
    Q_PROPERTY(QJsonObject arguments READ arguments WRITE setArguments)

    /*!
        \property QMcpGetPromptRequestParams::name
        \brief The name of the prompt or prompt template.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

public:
    QMcpGetPromptRequestParams() : QMcpGadget(new Private) {}

    QJsonObject arguments() const {
        return d<Private>()->arguments;
    }

    void setArguments(const QJsonObject &value) {
        if (arguments() == value) return;
        d<Private>()->arguments = value;
    }

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &value) {
        if (name() == value) return;
        d<Private>()->name = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject arguments;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpGetPromptRequestParams)

QT_END_NAMESPACE

#endif // QMCPGETPROMPTREQUESTPARAMS_H
