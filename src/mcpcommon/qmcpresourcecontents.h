// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCECONTENTS_H
#define QMCPRESOURCECONTENTS_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceContents
    \inmodule QtMcpCommon
    \brief The contents of a specific resource or sub-resource.
*/
class Q_MCPCOMMON_EXPORT QMcpResourceContents : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpResourceContents::_meta
        \brief Reserved by MCP to allow clients and servers to attach additional metadata.
        \since MCP 2025-06-18
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    /*!
        \property QMcpResourceContents::mimeType
        \brief The MIME type of this resource, if known.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
        \property QMcpResourceContents::uri
        \brief The URI of this resource.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

public:
    QMcpResourceContents() : QMcpGadget(new Private) {}

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
        if (name == "_meta")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_06_18;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    struct Private : public QMcpGadget::Private {
        QJsonObject _meta;
        QString mimeType;
        QUrl uri;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpResourceContents)

QT_END_NAMESPACE

#endif // QMCPRESOURCECONTENTS_H
