// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCETEMPLATE_H
#define QMCPRESOURCETEMPLATE_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpannotations.h>
#include <QtMcpCommon/qmcpicon.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceTemplate
    \inmodule QtMcpCommon
    \brief A template description for resources available on the server.
*/
class Q_MCPCOMMON_EXPORT QMcpResourceTemplate : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)

    /*!
        \property QMcpResourceTemplate::description
        \brief A description of what this template is for.
        
        This can be used by clients to improve the LLM's understanding of available resources. It can be thought of like a "hint" to the model.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpResourceTemplate::icons
        \brief An optional set of sized icons that the client can display in a user interface.

        \since MCP 2025-11-25
    */
    Q_PROPERTY(QList<QMcpIcon> icons READ icons WRITE setIcons)

    /*!
        \property QMcpResourceTemplate::mimeType
        \brief The MIME type for all resources that match this template. This should only be included if all resources matching this template have the same type.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
        \property QMcpResourceTemplate::name
        \brief A human-readable name for the type of resource this template refers to.
        
        This can be used by clients to populate UI elements.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    /*!
        \property QMcpResourceTemplate::title
        \brief Intended for UI and end-user contexts.

        Optimized to be human-readable and easily understood, even by those
        unfamiliar with domain-specific terminology. If not provided, the name
        should be used for display.

        \since MCP 2025-06-18
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    /*!
        \property QMcpResourceTemplate::uriTemplate
        \brief A URI template (according to RFC 6570) that can be used to construct resource URIs.
    */
    Q_PROPERTY(QString uriTemplate READ uriTemplate WRITE setUriTemplate REQUIRED)

public:
    QMcpResourceTemplate() : QMcpGadget(new Private) {
        qRegisterMetaType<QMcpIcon>();
    }

    QMcpAnnotations annotations() const {
        return d<Private>()->annotations;
    }

    void setAnnotations(const QMcpAnnotations &annotations) {
        if (this->annotations() == annotations) return;
        d<Private>()->annotations = annotations;
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

    QString mimeType() const {
        return d<Private>()->mimeType;
    }

    void setMimeType(const QString &mimeType) {
        if (this->mimeType() == mimeType) return;
        d<Private>()->mimeType = mimeType;
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

    QString uriTemplate() const {
        return d<Private>()->uriTemplate;
    }

    void setUriTemplate(const QString &uriTemplate) {
        if (this->uriTemplate() == uriTemplate) return;
        d<Private>()->uriTemplate = uriTemplate;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "icons")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
        if (name == "title")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QMcpAnnotations annotations;
        QString description;
        QList<QMcpIcon> icons;
        QString mimeType;
        QString name;
        QString title;
        QString uriTemplate;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpResourceTemplate)

QT_END_NAMESPACE

#endif // QMCPRESOURCETEMPLATE_H
