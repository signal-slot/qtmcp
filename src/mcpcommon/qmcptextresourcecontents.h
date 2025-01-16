// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTEXTRESOURCECONTENTS_H
#define QMCPTEXTRESOURCECONTENTS_H

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTextResourceContents
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpTextResourceContents : public QMcpGadget
{
    Q_GADGET

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

private:
    struct Private : public QMcpGadget::Private {
        QString mimeType;
        QString text;
        QUrl uri;
        QString name;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPTEXTRESOURCECONTENTS_H
