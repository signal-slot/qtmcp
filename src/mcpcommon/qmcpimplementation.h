// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPIMPLEMENTATION_H
#define QMCPIMPLEMENTATION_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpicon.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpImplementation
    \inmodule QtMcpCommon
    \brief Describes the name and version of an MCP implementation.
*/
class Q_MCPCOMMON_EXPORT QMcpImplementation : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpImplementation::description
        \brief An optional human-readable description of what this implementation does.

        This can be used by clients or servers to provide context about their
        purpose and capabilities. For example, a server might describe the types
        of resources or tools it provides, while a client might describe its
        intended use case.

        \since MCP 2025-11-25
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpImplementation::icons
        \brief An optional set of sized icons that the client can display in a user interface.

        \since MCP 2025-11-25
    */
    Q_PROPERTY(QList<QMcpIcon> icons READ icons WRITE setIcons)

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

    /*!
        \property QMcpImplementation::websiteUrl
        \brief An optional URL of the website for this implementation.

        \since MCP 2025-11-25
    */
    Q_PROPERTY(QUrl websiteUrl READ websiteUrl WRITE setWebsiteUrl)

public:
    QMcpImplementation() : QMcpGadget(new Private) {
        qRegisterMetaType<QMcpIcon>();
    }

    QString description() const {
        return d<Private>()->description;
    }

    void setDescription(const QString &description) {
        if (this->description() == description) return;
        d<Private>()->description = description;
    }

    QList<QMcpIcon> icons() const {
        return d<Private>()->icons;
    }

    void setIcons(const QList<QMcpIcon> &icons) {
        if (this->icons() == icons) return;
        d<Private>()->icons = icons;
    }

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

    QUrl websiteUrl() const {
        return d<Private>()->websiteUrl;
    }

    void setWebsiteUrl(const QUrl &websiteUrl) {
        if (this->websiteUrl() == websiteUrl) return;
        d<Private>()->websiteUrl = websiteUrl;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "description" || name == "icons" || name == "websiteUrl")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
        if (name == "title")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QString description;
        QList<QMcpIcon> icons;
        QString name;
        QString title;
        QString version;
        QUrl websiteUrl;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpImplementation)

QT_END_NAMESPACE

#endif // QMCPIMPLEMENTATION_H
