// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESOURCEREFERENCE_H
#define QMCPRESOURCEREFERENCE_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResourceReference
    \inmodule QtMcpCommon
    \brief A reference to a resource or resource template definition.
*/
class Q_MCPCOMMON_EXPORT QMcpResourceReference : public QMcpGadget
{
    Q_GADGET

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

    /*!
        \property QMcpResourceReference::uri
        \brief The URI or URI template of the resource.
    */
    Q_PROPERTY(QString uri READ uri WRITE setUri REQUIRED)

public:
    QMcpResourceReference() : QMcpGadget(new Private) {}

    QByteArray type() const {
        return d<Private>()->type;
    }

    QString uri() const {
        return d<Private>()->uri;
    }

    void setUri(const QString &uri) {
        if (this->uri() == uri) return;
        d<Private>()->uri = uri;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        const QByteArray type = QByteArrayLiteral("ref/resource");
        QString uri;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESOURCEREFERENCE_H
