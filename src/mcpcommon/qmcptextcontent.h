// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTEXTCONTENT_H
#define QMCPTEXTCONTENT_H

#include <QtCore/QByteArray>
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

private:
    struct Private : public QMcpGadget::Private {
        QMcpAnnotations annotations;
        QString text;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTextContent)

QT_END_NAMESPACE

#endif // QMCPTEXTCONTENT_H
