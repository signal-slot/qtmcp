// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPMODELPREFERENCES_H
#define QMCPMODELPREFERENCES_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpmodelhint.h>
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

/*! \class QMcpModelPreferences
    \inmodule QtMcpCommon
    \brief The server's preferences for model selection, requested of the client during sampling.
    
    Because LLMs can vary along multiple dimensions, choosing the "best" model is
    rarely straightforward.  Different models excel in different areas—some are
    faster but less capable, others are more capable but more expensive, and so
    on. This interface allows servers to express their priorities across multiple
    dimensions to help clients make an appropriate selection for their use case.
    
    These preferences are always advisory. The client MAY ignore them. It is also
    up to the client to decide how to interpret these preferences and how to
    balance them against other considerations.
*/
class Q_MCPCOMMON_EXPORT QMcpModelPreferences : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpModelPreferences::costPriority
        \brief How much to prioritize cost when selecting a model. A value of 0 means cost
        is not important, while a value of 1 means cost is the most important
        factor.

        The default value is 0.
    */
    Q_PROPERTY(qreal costPriority READ costPriority WRITE setCostPriority)

    /*!
        \property QMcpModelPreferences::hints
        \brief Optional hints to use for model selection.
        
        If multiple hints are specified, the client MUST evaluate them in order
        (such that the first match is taken).
        
        The client SHOULD prioritize these hints over the numeric priorities, but
        MAY still use the priorities to select from ambiguous matches.
    */
    Q_PROPERTY(QList<QMcpModelHint> hints READ hints WRITE setHints)

    /*!
        \property QMcpModelPreferences::intelligencePriority
        \brief How much to prioritize intelligence and capabilities when selecting a
        model. A value of 0 means intelligence is not important, while a value of 1
        means intelligence is the most important factor.

        The default value is 0.
    */
    Q_PROPERTY(qreal intelligencePriority READ intelligencePriority WRITE setIntelligencePriority)

    /*!
        \property QMcpModelPreferences::speedPriority
        \brief How much to prioritize sampling speed (latency) when selecting a model. A
        value of 0 means speed is not important, while a value of 1 means speed is
        the most important factor.

        The default value is 0.
    */
    Q_PROPERTY(qreal speedPriority READ speedPriority WRITE setSpeedPriority)

public:
    QMcpModelPreferences() : QMcpGadget(new Private) {}

    qreal costPriority() const {
        return d<Private>()->costPriority;
    }

    void setCostPriority(qreal costPriority) {
        if (this->costPriority() == costPriority) return;
        d<Private>()->costPriority = costPriority;
    }

    QList<QMcpModelHint> hints() const {
        return d<Private>()->hints;
    }

    void setHints(const QList<QMcpModelHint> &hints) {
        if (this->hints() == hints) return;
        d<Private>()->hints = hints;
    }

    qreal intelligencePriority() const {
        return d<Private>()->intelligencePriority;
    }

    void setIntelligencePriority(qreal intelligencePriority) {
        if (this->intelligencePriority() == intelligencePriority) return;
        d<Private>()->intelligencePriority = intelligencePriority;
    }

    qreal speedPriority() const {
        return d<Private>()->speedPriority;
    }

    void setSpeedPriority(qreal speedPriority) {
        if (this->speedPriority() == speedPriority) return;
        d<Private>()->speedPriority = speedPriority;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }
public:
    struct Private : public QMcpGadget::Private {
        qreal costPriority = 0;
        QList<QMcpModelHint> hints;
        qreal intelligencePriority = 0;
        qreal speedPriority = 0;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPMODELPREFERENCES_H
