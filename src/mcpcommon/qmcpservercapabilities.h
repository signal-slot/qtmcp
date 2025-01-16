// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERCAPABILITIES_H
#define QMCPSERVERCAPABILITIES_H

#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpservercapabilitiesexperimental.h>
#include <QtMcpCommon/qmcpservercapabilitieslogging.h>
#include <QtMcpCommon/qmcpservercapabilitiesprompts.h>
#include <QtMcpCommon/qmcpservercapabilitiesresources.h>
#include <QtMcpCommon/qmcpservercapabilitiestools.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpServerCapabilities
    \inmodule QtMcpCommon
    \brief Capabilities that a server may support. Known capabilities are defined here, in this schema, but this is not a closed set: any server can define its own, additional capabilities.
*/
class Q_MCPCOMMON_EXPORT QMcpServerCapabilities : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpServerCapabilities::experimental
        \brief Experimental, non-standard capabilities that the server supports.
    */
    Q_PROPERTY(QMcpServerCapabilitiesExperimental experimental READ experimental WRITE setExperimental)

    /*!
        \property QMcpServerCapabilities::logging
        \brief Present if the server supports sending log messages to the client.
    */
    Q_PROPERTY(QMcpServerCapabilitiesLogging logging READ logging WRITE setLogging)

    /*!
        \property QMcpServerCapabilities::prompts
        \brief Present if the server offers any prompt templates.
    */
    Q_PROPERTY(QMcpServerCapabilitiesPrompts prompts READ prompts WRITE setPrompts)

    /*!
        \property QMcpServerCapabilities::resources
        \brief Present if the server offers any resources to read.
    */
    Q_PROPERTY(QMcpServerCapabilitiesResources resources READ resources WRITE setResources)

    /*!
        \property QMcpServerCapabilities::tools
        \brief Present if the server offers any tools to call.
    */
    Q_PROPERTY(QMcpServerCapabilitiesTools tools READ tools WRITE setTools)

public:
    QMcpServerCapabilities() : QMcpGadget(new Private) {}

    QMcpServerCapabilitiesExperimental experimental() const {
        return d<Private>()->experimental;
    }

    void setExperimental(const QMcpServerCapabilitiesExperimental &experimental) {
        if (this->experimental() == experimental) return;
        d<Private>()->experimental = experimental;
    }

    QMcpServerCapabilitiesLogging logging() const {
        return d<Private>()->logging;
    }

    void setLogging(const QMcpServerCapabilitiesLogging &logging) {
        if (this->logging() == logging) return;
        d<Private>()->logging = logging;
    }

    QMcpServerCapabilitiesPrompts prompts() const {
        return d<Private>()->prompts;
    }

    void setPrompts(const QMcpServerCapabilitiesPrompts &prompts) {
        if (this->prompts() == prompts) return;
        d<Private>()->prompts = prompts;
    }

    QMcpServerCapabilitiesResources resources() const {
        return d<Private>()->resources;
    }

    void setResources(const QMcpServerCapabilitiesResources &resources) {
        if (this->resources() == resources) return;
        d<Private>()->resources = resources;
    }

    QMcpServerCapabilitiesTools tools() const {
        return d<Private>()->tools;
    }

    void setTools(const QMcpServerCapabilitiesTools &tools) {
        if (this->tools() == tools) return;
        d<Private>()->tools = tools;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpServerCapabilitiesExperimental experimental;
        QMcpServerCapabilitiesLogging logging;
        QMcpServerCapabilitiesPrompts prompts;
        QMcpServerCapabilitiesResources resources;
        QMcpServerCapabilitiesTools tools;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPSERVERCAPABILITIES_H
