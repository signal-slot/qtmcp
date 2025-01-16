// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPLOGGINGMESSAGENOTIFICATIONPARAMS_H
#define QMCPLOGGINGMESSAGENOTIFICATIONPARAMS_H

#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcplogginglevel.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpLoggingMessageNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpLoggingMessageNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpLoggingMessageNotificationParams::data
        \brief The data to be logged, such as a string message or an object. Any JSON serializable type is allowed here.
    */
    Q_PROPERTY(QJsonValue data READ data WRITE setData REQUIRED)

    /*!
        \property QMcpLoggingMessageNotificationParams::level
        \brief The severity of this log message.
    */
    Q_PROPERTY(QMcpLoggingLevel::QMcpLoggingLevel level READ level WRITE setLevel REQUIRED)

    /*!
        \property QMcpLoggingMessageNotificationParams::logger
        \brief An optional name of the logger issuing this message.
    */
    Q_PROPERTY(QString logger READ logger WRITE setLogger)

public:
    QMcpLoggingMessageNotificationParams() : QMcpNotificationParams(new Private) {}

    QJsonValue data() const {
        return d<Private>()->data;
    }

    void setData(const QJsonValue &data) {
        if (this->data() == data) return;
        d<Private>()->data = data;
    }

    QMcpLoggingLevel::QMcpLoggingLevel level() const {
        return d<Private>()->level;
    }

    void setLevel(QMcpLoggingLevel::QMcpLoggingLevel level) {
        if (this->level() == level) return;
        d<Private>()->level = level;
    }

    QString logger() const {
        return d<Private>()->logger;
    }

    void setLogger(const QString &logger) {
        if (this->logger() == logger) return;
        d<Private>()->logger = logger;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        QJsonValue data;
        QMcpLoggingLevel::QMcpLoggingLevel level = QMcpLoggingLevel::alert;
        QString logger;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPLOGGINGMESSAGENOTIFICATIONPARAMS_H
