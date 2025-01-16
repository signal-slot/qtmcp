// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPCLIENTCAPABILITIES_H
#define QMCPCLIENTCAPABILITIES_H

#include <QtMcpCommon/qmcpclientcapabilitiesexperimental.h>
#include <QtMcpCommon/qmcpclientcapabilitiesroots.h>
#include <QtMcpCommon/qmcpclientcapabilitiessampling.h>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpClientCapabilities
    \inmodule QtMcpCommon
    \brief Capabilities a client may support. Known capabilities are defined here, in this schema, but this is not a closed set: any client can define its own, additional capabilities.
*/
class Q_MCPCOMMON_EXPORT QMcpClientCapabilities : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpClientCapabilities::experimental
        \brief Experimental, non-standard capabilities that the client supports.
    */
    Q_PROPERTY(QMcpClientCapabilitiesExperimental experimental READ experimental WRITE setExperimental)

    /*!
        \property QMcpClientCapabilities::roots
        \brief Present if the client supports listing roots.
    */
    Q_PROPERTY(QMcpClientCapabilitiesRoots roots READ roots WRITE setRoots)

    /*!
        \property QMcpClientCapabilities::sampling
        \brief Present if the client supports sampling from an LLM.
    */
    Q_PROPERTY(QMcpClientCapabilitiesSampling sampling READ sampling WRITE setSampling)

public:
    QMcpClientCapabilities() : QMcpGadget(new Private) {}

    QMcpClientCapabilitiesExperimental experimental() const {
        return d<Private>()->experimental;
    }

    void setExperimental(const QMcpClientCapabilitiesExperimental &experimental) {
        if (this->experimental() == experimental) return;
        d<Private>()->experimental = experimental;
    }

    QMcpClientCapabilitiesRoots roots() const {
        return d<Private>()->roots;
    }

    void setRoots(const QMcpClientCapabilitiesRoots &roots) {
        if (this->roots() == roots) return;
        d<Private>()->roots = roots;
    }

    QMcpClientCapabilitiesSampling sampling() const {
        return d<Private>()->sampling;
    }

    void setSampling(const QMcpClientCapabilitiesSampling &sampling) {
        if (this->sampling() == sampling) return;
        d<Private>()->sampling = sampling;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpClientCapabilitiesExperimental experimental;
        QMcpClientCapabilitiesRoots roots;
        QMcpClientCapabilitiesSampling sampling;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPCLIENTCAPABILITIES_H
