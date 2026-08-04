// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTOOLCHOICE_H
#define QMCPTOOLCHOICE_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpToolChoice
    \inmodule QtMcpCommon
    \brief Controls tool selection behavior for sampling requests.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpToolChoice : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpToolChoice::mode
        \brief Controls the tool use ability of the model.

        \list
            \li "auto": Model decides whether to use tools (default)
            \li "required": Model MUST use at least one tool before completing
            \li "none": Model MUST NOT use any tools
        \endlist

        Like QMcpCreateMessageRequestParams::includeContext() this is kept as a
        string rather than an enum: "auto" is a C++ keyword, so the enumerators
        could not be named after the values used on the wire.
    */
    Q_PROPERTY(QString mode READ mode WRITE setMode)

public:
    QMcpToolChoice() : QMcpGadget(new Private) {}

    QString mode() const {
        return d<Private>()->mode;
    }

    void setMode(const QString &mode) {
        if (this->mode() == mode) return;
        d<Private>()->mode = mode;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString mode;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpToolChoice)

QT_END_NAMESPACE

#endif // QMCPTOOLCHOICE_H
