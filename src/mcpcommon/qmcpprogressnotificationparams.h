// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPPROGRESSNOTIFICATIONPARAMS_H
#define QMCPPROGRESSNOTIFICATIONPARAMS_H

#include <QtMcpCommon/qmcpnotificationparams.h>
#include <QtMcpCommon/qmcpprogresstoken.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpProgressNotificationParams
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpProgressNotificationParams : public QMcpNotificationParams
{
    Q_GADGET

    /*!
        \property QMcpProgressNotificationParams::progress
        \brief The progress thus far. This should increase every time progress is made, even if the total is unknown.

        The default value is 0.
    */
    Q_PROPERTY(qreal progress READ progress WRITE setProgress REQUIRED)

    /*!
        \property QMcpProgressNotificationParams::progressToken
        \brief The progress token which was given in the initial request, used to associate this notification with the request that is proceeding.
    */
    Q_PROPERTY(QMcpProgressToken progressToken READ progressToken WRITE setProgressToken REQUIRED)

    /*!
        \property QMcpProgressNotificationParams::total
        \brief Total number of items to process (or total progress required), if known.

        The default value is 0.
    */
    Q_PROPERTY(qreal total READ total WRITE setTotal)

public:
    QMcpProgressNotificationParams() : QMcpNotificationParams(new Private) {}

    qreal progress() const {
        return d<Private>()->progress;
    }

    void setProgress(qreal progress) {
        if (qFuzzyCompare(this->progress(), progress)) return;
        d<Private>()->progress = progress;
    }

    QMcpProgressToken progressToken() const {
        return d<Private>()->progressToken;
    }

    void setProgressToken(const QMcpProgressToken &token) {
        if (this->progressToken() == token) return;
        d<Private>()->progressToken = token;
    }

    qreal total() const {
        return d<Private>()->total;
    }

    void setTotal(qreal total) {
        if (qFuzzyCompare(this->total(), total)) return;
        d<Private>()->total = total;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpNotificationParams::Private {
        qreal progress = 0;
        QMcpProgressToken progressToken;
        qreal total = 0;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpProgressNotificationParams)

QT_END_NAMESPACE

#endif // QMCPPROGRESSNOTIFICATIONPARAMS_H
