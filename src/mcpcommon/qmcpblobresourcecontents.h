// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPBLOBRESOURCECONTENTS_H
#define QMCPBLOBRESOURCECONTENTS_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpBlobResourceContents
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpBlobResourceContents : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpBlobResourceContents::blob
        \brief A base64-encoded string representing the binary data of the item.
    */
    Q_PROPERTY(QByteArray blob READ blob WRITE setBlob REQUIRED)

    /*!
        \property QMcpBlobResourceContents::mimeType
        \brief The MIME type of this resource, if known.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
        \property QMcpBlobResourceContents::uri
        \brief The URI of this resource.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QMcpBlobResourceContents() : QMcpGadget(new Private) {}

    QByteArray blob() const {
        return d<Private>()->blob;
    }

    void setBlob(const QByteArray &blob) {
        if (this->blob() == blob) return;
        d<Private>()->blob = blob;
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

private:
    struct Private : public QMcpGadget::Private {
    public:
        QByteArray blob;
        QString mimeType;
        QUrl uri;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPBLOBRESOURCECONTENTS_H
