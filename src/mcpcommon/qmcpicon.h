// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPICON_H
#define QMCPICON_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpIcon
    \inmodule QtMcpCommon
    \brief An optionally-sized icon that can be displayed in a user interface.

    \since MCP 2025-11-25
*/
class Q_MCPCOMMON_EXPORT QMcpIcon : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpIcon::mimeType
        \brief Optional MIME type override if the source MIME type is missing or generic.

        For example: "image/png", "image/jpeg", or "image/svg+xml".
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType)

    /*!
        \property QMcpIcon::sizes
        \brief Optional list of sizes at which the icon can be used.

        Each string should be in WxH format (e.g. "48x48", "96x96") or "any" for
        scalable formats like SVG. If empty, the client should assume that the
        icon can be used at any size.
    */
    Q_PROPERTY(QList<QString> sizes READ sizes WRITE setSizes)

    /*!
        \property QMcpIcon::src
        \brief A standard URI pointing to an icon resource.

        May be an HTTP/HTTPS URL or a data: URI with Base64-encoded image data.
        Consumers should ensure URLs serving icons are from the same domain as
        the client/server or a trusted domain, and should take appropriate
        precautions when consuming SVGs as they can contain executable
        JavaScript.
    */
    Q_PROPERTY(QUrl src READ src WRITE setSrc REQUIRED)

    /*!
        \property QMcpIcon::theme
        \brief Optional specifier for the theme this icon is designed for.

        "light" indicates the icon is designed to be used with a light
        background, and "dark" indicates the icon is designed to be used with a
        dark background. If empty, the client should assume the icon can be used
        with any theme.
    */
    Q_PROPERTY(QString theme READ theme WRITE setTheme)

public:
    QMcpIcon() : QMcpGadget(new Private) {}

    QString mimeType() const {
        return d<Private>()->mimeType;
    }

    void setMimeType(const QString &mimeType) {
        if (this->mimeType() == mimeType) return;
        d<Private>()->mimeType = mimeType;
    }

    QList<QString> sizes() const {
        return d<Private>()->sizes;
    }

    void setSizes(const QList<QString> &sizes) {
        if (this->sizes() == sizes) return;
        d<Private>()->sizes = sizes;
    }

    QUrl src() const {
        return d<Private>()->src;
    }

    void setSrc(const QUrl &src) {
        if (this->src() == src) return;
        d<Private>()->src = src;
    }

    QString theme() const {
        return d<Private>()->theme;
    }

    void setTheme(const QString &theme) {
        if (this->theme() == theme) return;
        d<Private>()->theme = theme;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString mimeType;
        QList<QString> sizes;
        QUrl src;
        QString theme;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpIcon)

QT_END_NAMESPACE

#endif // QMCPICON_H
