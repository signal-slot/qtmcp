// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCETEMPLATE_H
#define QMCPRESOURCETEMPLATE_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpannotations.h>

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
        \property QMcpResourceTemplate::uriTemplate
        \brief A URI template (according to RFC 6570) that can be used to construct resource URIs.
    */
    Q_PROPERTY(QString uriTemplate READ uriTemplate WRITE setUriTemplate REQUIRED)

public:
    QMcpResourceTemplate() : QMcpGadget(new Private) {}

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

private:
    struct Private : public QMcpGadget::Private {
        QMcpAnnotations annotations;
        QString description;
        QString mimeType;
        QString name;
        QString uriTemplate;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESOURCETEMPLATE_H
