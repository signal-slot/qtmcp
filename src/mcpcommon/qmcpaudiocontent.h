// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPAUDIOCONTENT_H
#define QMCPAUDIOCONTENT_H

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpannotations.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpAudioContent
    \inmodule QtMcpCommon
    \brief Audio provided to or from an LLM.
*/
class Q_MCPCOMMON_EXPORT QMcpAudioContent : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpAudioContent::type
        \brief The type of content, always "audio".
    */
    Q_PROPERTY(QString type READ type CONSTANT REQUIRED)

    /*!
        \property QMcpAudioContent::data
        \brief The base64-encoded audio data.
    */
    Q_PROPERTY(QByteArray data READ data WRITE setData REQUIRED)

    /*!
        \property QMcpAudioContent::mimeType
        \brief The MIME type of the audio. Different providers may support different audio types.
    */
    Q_PROPERTY(QString mimeType READ mimeType WRITE setMimeType REQUIRED)

    /*!
        \property QMcpAudioContent::annotations
        \brief Optional annotations for the client.
    */
    Q_PROPERTY(QMcpAnnotations annotations READ annotations WRITE setAnnotations)

public:
    QMcpAudioContent() : QMcpGadget(new Private) {}

    QString type() const {
        return "audio"_L1;
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

    QMcpAnnotations annotations() const {
        return d<Private>()->annotations;
    }

    void setAnnotations(const QMcpAnnotations &annotations) {
        if (this->annotations() == annotations) return;
        d<Private>()->annotations = annotations;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        QByteArray data;
        QString mimeType;
        QMcpAnnotations annotations;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpAudioContent)

QT_END_NAMESPACE

#endif // QMCPAUDIOCONTENT_H