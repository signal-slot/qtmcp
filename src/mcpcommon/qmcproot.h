// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPROOT_H
#define QMCPROOT_H

#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpRoot
    \inmodule QtMcpCommon
    \brief Represents a root directory or file that the server can operate on.
*/
class Q_MCPCOMMON_EXPORT QMcpRoot : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpRoot::name
        \brief An optional name for the root. This can be used to provide a human-readable
        identifier for the root, which may be useful for display purposes or for
        referencing the root in other parts of the application.
    */
    Q_PROPERTY(QString name READ name WRITE setName)

    /*!
        \property QMcpRoot::uri
        \brief The URI identifying the root. This *must* start with file:// for now.
        This restriction may be relaxed in future versions of the protocol to allow
        other URI schemes.
    */
    Q_PROPERTY(QUrl uri READ uri WRITE setUri REQUIRED)

public:
    QMcpRoot() : QMcpGadget(new Private) {}

    QString name() const {
        return d<Private>()->name;
    }

    void setName(const QString &name) {
        if (this->name() == name) return;
        d<Private>()->name = name;
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
        QString name;
        QUrl uri;

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPROOT_H
