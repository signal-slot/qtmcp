// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCELINK_H
#define QMCPRESOURCELINK_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpannotations.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceLink
    \inmodule QtMcpCommon
    \brief A resource that the server is capable of reading, included in a prompt or tool call result.

    Note: resource links returned by tools are not guaranteed to appear in the
    results of `resources/list` requests.

    \since MCP 2025-06-18
*/
class Q_MCPCOMMON_EXPORT QMcpResourceLink : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpResourceLink::_meta
        \brief Reserved by MCP to allow clients and servers to attach additional metadata.
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)

    /*!
        \property QMcpResourceLink::description
        \brief A description of what this resource represents.

        This can be used by clients to improve the LLM's understanding of available resources. It can be thought of like a "hint" to the model.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpResourceLink::mimeType
        \brief The MIME type of this resource, if known.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
        \property QMcpResourceLink::name
        \brief Intended for programmatic or logical use, but used as a display name in past specs or fallback (if title isn't present).
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    /*!
        \property QMcpResourceLink::size
        \brief The size of the raw resource content, in bytes (i.e., before base64 encoding or any tokenization), if known.

        This can be used by Hosts to display file sizes and estimate context window usage.

        The default value is 0.
    */
    Q_PROPERTY(int size READ size WRITE setSize)

    /*!
        \property QMcpResourceLink::title
        \brief Intended for UI and end-user contexts.

        Optimized to be human-readable and easily understood, even by those
        unfamiliar with domain-specific terminology. If not provided, the name
        should be used for display.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

    /*!
        \property QMcpResourceLink::uri
        \brief The URI of this resource.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

public:
    QMcpResourceLink() : QMcpGadget(new Private) {}

    QJsonObject meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QJsonObject &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
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

    int size() const {
        return d<Private>()->size;
    }

    void setSize(int size) {
        if (this->size() == size) return;
        d<Private>()->size = size;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    static QByteArray type() { return QByteArrayLiteral("resource_link"); }

    QUrl uri() const {
        return d<Private>()->uri;
    }

    void setUri(const QUrl &uri) {
        if (this->uri() == uri) return;
        d<Private>()->uri = uri;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        // ResourceLink itself has been added in 2025-06-18. The gate is kept
        // anyway so that a resource link never carries 2025-06-18 only fields
        // when it is serialized for an older revision.
        if (name == "title" || name == "_meta")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject _meta;
        QMcpAnnotations annotations;
        QString description;
        QString mimeType;
        QString name;
        int size = 0;
        QString title;
        QUrl uri;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpResourceLink)

QT_END_NAMESPACE

#endif // QMCPRESOURCELINK_H
