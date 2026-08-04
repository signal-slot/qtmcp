// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPEMBEDDEDRESOURCE_H
#define QMCPEMBEDDEDRESOURCE_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpannotations.h>
#include <QtMcpCommon/qmcpembeddedresourceresource.h>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpEmbeddedResource
    \inmodule QtMcpCommon
    \brief The contents of a resource, embedded into a prompt or tool call result.
    
    It is up to the client how best to render embedded resources for the benefit
    of the LLM and/or the user.
*/
class Q_MCPCOMMON_EXPORT QMcpEmbeddedResource : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpEmbeddedResource::_meta
        \brief Reserved by MCP to allow clients and servers to attach additional metadata.
        \since MCP 2025-06-18
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)
    Q_PROPERTY(QMcpEmbeddedResourceResource resource READ resource WRITE setResource REQUIRED)
    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpEmbeddedResource() : QMcpGadget(new Private) {}

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

    QMcpEmbeddedResourceResource resource() const {
        return d<Private>()->resource;
    }

    void setResource(const QMcpEmbeddedResourceResource &resource) {
        if (this->resource() == resource) return;
        d<Private>()->resource = resource;
    }

    static QByteArray type() { return QByteArrayLiteral("resource"); }

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
    public:
        QJsonObject _meta;
        QMcpAnnotations annotations;
        QMcpEmbeddedResourceResource resource;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpEmbeddedResource)

QT_END_NAMESPACE

#endif // QMCPEMBEDDEDRESOURCE_H
