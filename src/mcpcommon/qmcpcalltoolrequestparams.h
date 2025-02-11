// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCALLTOOLREQUESTPARAMS_H
#define QMCPCALLTOOLREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpCallToolRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpCallToolRequestParams : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QJsonObject arguments READ arguments WRITE setArguments)
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

public:
    QMcpCallToolRequestParams() : QMcpGadget(new Private) {}

    QJsonObject arguments() const {
        return d<Private>()->arguments;
    }

    void setArguments(const QJsonObject &arguments) {
        if (this->arguments() == arguments) return;
        d<Private>()->arguments = arguments;
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
    public:
        QJsonObject arguments;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpCallToolRequestParams)

QT_END_NAMESPACE

#endif // QMCPCALLTOOLREQUESTPARAMS_H
