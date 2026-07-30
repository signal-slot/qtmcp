// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPIMAGECONTENT_H
#define QMCPIMAGECONTENT_H

#include <QtCore/QByteArray>
#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtMcpCommon/qmcpannotations.h>
#include <QtMcpCommon/qmcpgadget.h>

#ifdef QT_GUI_LIB
#include <QtCore/QBuffer>
#include <QtGui/QImage>
#endif

QT_BEGIN_NAMESPACE

/*! \class QMcpImageContent
    \inmodule QtMcpCommon
    \brief An image provided to or from an LLM.
*/
class Q_MCPCOMMON_EXPORT QMcpImageContent : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpImageContent::_meta
        \brief Reserved by MCP to allow clients and servers to attach additional metadata.
        \since MCP 2025-06-18
    */
    Q_PROPERTY(QJsonObject _meta READ meta WRITE setMeta)

    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)

    /*!
        \property QMcpImageContent::data
        \brief The base64-encoded image data.
    */
    Q_PROPERTY(QByteArray data READ data WRITE setData REQUIRED)

    /*!
        \property QMcpImageContent::mimeType
        \brief The MIME type of the image. Different providers may support different image types.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType REQUIRED)

    Q_PROPERTY(QByteArray type READ type CONSTANT REQUIRED)

public:
    QMcpImageContent() : QMcpGadget(new Private) {}
#ifdef QT_GUI_LIB
    QMcpImageContent(const QImage &image)
        : QMcpGadget(new Private) {
        setMimeType(QStringLiteral("image/png"));
        QByteArray data;
        QBuffer buffer(&data);
        if (buffer.open(QBuffer::WriteOnly)) {
            if (image.save(&buffer, "PNG")) {
                setData(data.toBase64());
            }
        }
    }
#endif

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

    QByteArray data() const {
        return d<Private>()->data;
    }

    void setData(const QByteArray &data) {
        if (this->data() == data) return;
        d<Private>()->data = data;
    }

    QString mimeType() const {
        return d<Private>()->mimeType;
    }

    void setMimeType(const QString &mimeType) {
        if (this->mimeType() == mimeType) return;
        d<Private>()->mimeType = mimeType;
    }

    static QByteArray type() { return QByteArrayLiteral("image"); }

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
        QByteArray data;
        QString mimeType;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpImageContent)

QT_END_NAMESPACE

#endif // QMCPIMAGECONTENT_H
