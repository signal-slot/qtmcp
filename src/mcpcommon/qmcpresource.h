// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCE_H
#define QMCPRESOURCE_H

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpannotations.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResource
    \inmodule QtMcpCommon
    \brief A known resource that the server is capable of reading.
*/
class Q_MCPCOMMON_EXPORT QMcpResource : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)

    /*!
        \property QMcpResource::description
        \brief A description of what this resource represents.
        
        This can be used by clients to improve the LLM's understanding of available resources. It can be thought of like a "hint" to the model.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpResource::mimeType
        \brief The MIME type of this resource, if known.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
        \property QMcpResource::name
        \brief A human-readable name for this resource.
        
        This can be used by clients to populate UI elements.
    */
    Q_PROPERTY(QString name READ name WRITE setName REQUIRED)

    /*!
        \property QMcpResource::size
        \brief The size of the raw resource content, in bytes (i.e., before base64 encoding or any tokenization), if known.
        
        This can be used by Hosts to display file sizes and estimate context window usage.

        The default value is 0.
    */
    Q_PROPERTY(int size READ size WRITE setSize)

    /*!
        \property QMcpResource::uri
        \brief The URI of this resource.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

public:
    QMcpResource() : QMcpGadget(new Private) {}

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
        QMcpAnnotations annotations;
        QString description;
        QString mimeType;
        QString name;
        int size = 0;
        QUrl uri;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpResource)

QT_END_NAMESPACE

#endif // QMCPRESOURCE_H
