// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCECONTENTS_H
#define QMCPRESOURCECONTENTS_H

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

private:
    struct Private : public QMcpGadget::Private {
        QString mimeType;
        QUrl uri;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESOURCECONTENTS_H
