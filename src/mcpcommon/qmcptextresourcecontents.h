// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTEXTRESOURCECONTENTS_H
#define QMCPTEXTRESOURCECONTENTS_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpresource.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTextResourceContents
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpTextResourceContents : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpTextResourceContents::_meta
        \brief Reserved by MCP to allow clients and servers to attach additional metadata.
        \since MCP 2025-06-18
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    /*!
        \property QMcpTextResourceContents::mimeType
        \brief The MIME type of this resource, if known.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
        \property QMcpTextResourceContents::text
        \brief The text of the item. This must only be set if the item can actually be represented as text (not binary data).
    */
    Q_PROPERTY(QString text READ text WRITE setText REQUIRED)

    /*!
        \property QMcpTextResourceContents::uri
        \brief The URI of this resource.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QMcpTextResourceContents() : QMcpGadget(new Private) {}
    QMcpTextResourceContents(const QMcpResource &resource, const QString &text)
        : QMcpGadget(new Private)
    {
        d<Private>()->mimeType = resource.mimeType();
        d<Private>()->text = text;
        d<Private>()->uri = resource.uri();
        d<Private>()->name = resource.name();
    }

    QJsonObject meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QJsonObject &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    QString mimeType() const {
        return d<Private>()->mimeType;
    }

    void setMimeType(const QString &mimeType) {
        if (this->mimeType() == mimeType) return;
        d<Private>()->mimeType = mimeType;
    }

    QString text() const {
        return d<Private>()->text;
    }

    void setText(const QString &text) {
        if (this->text() == text) return;
        d<Private>()->text = text;
    }

    QUrl uri() const {
        return d<Private>()->uri;
    }

    void setUri(const QUrl &uri) {
        if (this->uri() == uri) return;
        d<Private>()->uri = uri;
    }

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "_meta")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject _meta;
        QString mimeType;
        QString text;
        QUrl uri;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTextResourceContents)

QT_END_NAMESPACE

#endif // QMCPTEXTRESOURCECONTENTS_H
