// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPIMPLEMENTATION_H
#define QMCPIMPLEMENTATION_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpImplementation
    \inmodule QtMcpCommon
    \brief Describes the name and version of an MCP implementation.
*/
class Q_MCPCOMMON_EXPORT QMcpImplementation : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    /*!
        \property QMcpImplementation::title
        \brief Intended for UI and end-user contexts.

        Optimized to be human-readable and easily understood, even by those
        unfamiliar with domain-specific terminology. If not provided, the name
        should be used for display.

        \since MCP 2025-06-18
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QString version READ version WRITE setVersion REQUIRED)

public:
    QMcpImplementation() : QMcpGadget(new Private) {}

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    QString version() const {
        return d<Private>()->version;
    }

    void setVersion(const QString &version) {
        if (this->version() == version) return;
        d<Private>()->version = version;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "title")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QString name;
        QString title;
        QString version;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpImplementation)

QT_END_NAMESPACE

#endif // QMCPIMPLEMENTATION_H
