// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCOMPLETEREQUESTPARAMSARGUMENT_H
#define QMCPCOMPLETEREQUESTPARAMSARGUMENT_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCompleteRequestParamsArgument
    \inmodule QtMcpCommon
    \brief The argument's information
*/
class Q_MCPCOMMON_EXPORT QMcpCompleteRequestParamsArgument : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpCompleteRequestParamsArgument::name
        \brief The name of the argument
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    /*!
        \property QMcpCompleteRequestParamsArgument::value
        \brief The value of the argument to use for completion matching.
    */
    Q_PROPERTY(QString value READ value WRITE setValue REQUIRED)

public:
    QMcpCompleteRequestParamsArgument() : QMcpGadget(new Private) {}

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    QString value() const {
        return d<Private>()->value;
    }

    void setValue(const QString &value) {
        if (this->value() == value) return;
        d<Private>()->value = value;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString name;
        QString value;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCompleteRequestParamsArgument)

QT_END_NAMESPACE

#endif // QMCPCOMPLETEREQUESTPARAMSARGUMENT_H
