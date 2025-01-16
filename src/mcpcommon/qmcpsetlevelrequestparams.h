// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSETLEVELREQUESTPARAMS_H
#define QMCPSETLEVELREQUESTPARAMS_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcplogginglevel.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpSetLevelRequestParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpSetLevelRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpSetLevelRequestParams::level
        \brief The level of logging that the client wants to receive from the server. The server should send all logs at this level and higher (i.e., more severe) to the client as notifications/logging/message.
    */
    Q_PROPERTY(QMcpLoggingLevel::QMcpLoggingLevel level READ level WRITE setLevel REQUIRED)

public:
    QMcpSetLevelRequestParams() : QMcpGadget(new Private) {}

    QMcpLoggingLevel::QMcpLoggingLevel level() const {
        return d<Private>()->level;
    }

    void setLevel(QMcpLoggingLevel::QMcpLoggingLevel level) {
        if (this->level() == level) return;
        d<Private>()->level = level;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpLoggingLevel::QMcpLoggingLevel level = QMcpLoggingLevel::alert;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPSETLEVELREQUESTPARAMS_H
