// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPELICITRESULT_H
#define QMCPELICITRESULT_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpresult.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpElicitResult
    \inmodule QtMcpCommon
    \brief The client's response to an elicitation request.

    This result has been introduced in the 2025-06-18 revision of the protocol.
*/
class Q_MCPCOMMON_EXPORT QMcpElicitResult : public QMcpResult
{
    Q_GADGET

    /*!
        \property QMcpElicitResult::action
        \brief The user action in response to the elicitation.

        \list
        \li "accept": User submitted the form/confirmed the action
        \li "cancel": User dismissed without making an explicit choice
        \li "decline": User explicitly declined the action
        \endlist
    */
    Q_PROPERTY(QString action READ action WRITE setAction REQUIRED)

    /*!
        \property QMcpElicitResult::content
        \brief The submitted form data, only present when action is "accept".

        Contains values matching the requested schema.
    */
    Q_PROPERTY(QJsonObject content READ content WRITE setContent)

public:
    QMcpElicitResult() : QMcpResult(new Private) {}

    QString action() const {
        return d<Private>()->action;
    }

    void setAction(const QString &action) {
        if (this->action() == action) return;
        d<Private>()->action = action;
    }

    QJsonObject content() const {
        return d<Private>()->content;
    }

    void setContent(const QJsonObject &content) {
        if (this->content() == content) return;
        d<Private>()->content = content;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpResult::Private {
        QString action;
        QJsonObject content;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpElicitResult)

QT_END_NAMESPACE

#endif // QMCPELICITRESULT_H
