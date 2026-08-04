// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTEXTCONTENT_H
#define QMCPTEXTCONTENT_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpannotations.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTextContent
    \inmodule QtMcpCommon
    \brief Text provided to or from an LLM.
*/
class Q_MCPCOMMON_EXPORT QMcpTextContent : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpTextContent::_meta
        \brief Reserved by MCP to allow clients and servers to attach additional metadata.
        \since MCP 2025-06-18
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)

    /*!
        \property QMcpTextContent::text
        \brief The text content of the message.
    */
    Q_PROPERTY(QString text READ text WRITE setText REQUIRED)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpTextContent() : QMcpGadget(new Private) {}
    QMcpTextContent(const QString &text) : QMcpGadget(new Private) { setText(text); }

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

    QString text() const {
        return d<Private>()->text;
    }

    void setText(const QString &text) {
        if (this->text() == text) return;
        d<Private>()->text = text;
    }

    static QByteArray type() { return QByteArrayLiteral("text"); }

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
        QJsonObject _meta;
        QMcpAnnotations annotations;
        QString text;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTextContent)

QT_END_NAMESPACE

#endif // QMCPTEXTCONTENT_H
