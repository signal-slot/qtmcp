// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSTRINGSCHEMA_H
#define QMCPSTRINGSCHEMA_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpStringSchema
    \inmodule QtMcpCommon
    \brief A restricted JSON Schema for a string value requested via elicitation.

    \sa QMcpPrimitiveSchemaDefinition
*/
class Q_MCPCOMMON_EXPORT QMcpStringSchema : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpStringSchema::description
        \brief A human-readable description of the requested value.
    */
    Q_PROPERTY(QString description READ description WRITE setDescription)

    /*!
        \property QMcpStringSchema::format
        \brief The expected format of the value.

        One of "date", "date-time", "email" or "uri". Empty if unconstrained.
    */
    Q_PROPERTY(QString format READ format WRITE setFormat)

    /*!
        \property QMcpStringSchema::maxLength
        \brief The largest accepted number of characters.

        The default value is 0.
    */
    Q_PROPERTY(int maxLength READ maxLength WRITE setMaxLength)

    /*!
        \property QMcpStringSchema::minLength
        \brief The smallest accepted number of characters.

        The default value is 0.
    */
    Q_PROPERTY(int minLength READ minLength WRITE setMinLength)

    /*!
        \property QMcpStringSchema::title
        \brief A human-readable title of the requested value.
    */
    Q_PROPERTY(QString title READ title WRITE setTitle)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpStringSchema() : QMcpGadget(new Private) {}

    QString description() const {
        return d<Private>()->description;
    }

    void setDescription(const QString &description) {
        if (this->description() == description) return;
        d<Private>()->description = description;
    }

    QString format() const {
        return d<Private>()->format;
    }

    void setFormat(const QString &format) {
        if (this->format() == format) return;
        d<Private>()->format = format;
    }

    int maxLength() const {
        return d<Private>()->maxLength;
    }

    void setMaxLength(int maxLength) {
        if (this->maxLength() == maxLength) return;
        d<Private>()->maxLength = maxLength;
    }

    int minLength() const {
        return d<Private>()->minLength;
    }

    void setMinLength(int minLength) {
        if (this->minLength() == minLength) return;
        d<Private>()->minLength = minLength;
    }

    QString title() const {
        return d<Private>()->title;
    }

    void setTitle(const QString &title) {
        if (this->title() == title) return;
        d<Private>()->title = title;
    }

    static QByteArray type() { return QByteArrayLiteral("string"); }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QString description;
        QString format;
        int maxLength = 0;
        int minLength = 0;
        QString title;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpStringSchema)

QT_END_NAMESPACE

#endif // QMCPSTRINGSCHEMA_H
