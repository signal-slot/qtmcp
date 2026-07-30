// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPELICITREQUESTPARAMS_H
#define QMCPELICITREQUESTPARAMS_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpelicitrequestparamsrequestedschema.h>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpElicitRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpElicitRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpElicitRequestParams::message
        \brief The message to present to the user.
    */
    Q_PROPERTY(QString message READ message WRITE setMessage REQUIRED)

    /*!
        \property QMcpElicitRequestParams::requestedSchema
        \brief A restricted subset of JSON Schema describing the requested values.
    */
    Q_PROPERTY(QMcpElicitRequestParamsRequestedSchema requestedSchema READ requestedSchema WRITE setRequestedSchema REQUIRED)

public:
    QMcpElicitRequestParams() : QMcpGadget(new Private) {}

    QString message() const {
        return d<Private>()->message;
    }

    void setMessage(const QString &message) {
        if (this->message() == message) return;
        d<Private>()->message = message;
    }

    QMcpElicitRequestParamsRequestedSchema requestedSchema() const {
        return d<Private>()->requestedSchema;
    }

    void setRequestedSchema(const QMcpElicitRequestParamsRequestedSchema &requestedSchema) {
        if (this->requestedSchema() == requestedSchema) return;
        d<Private>()->requestedSchema = requestedSchema;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString message;
        QMcpElicitRequestParamsRequestedSchema requestedSchema;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpElicitRequestParams)

QT_END_NAMESPACE

#endif // QMCPELICITREQUESTPARAMS_H
