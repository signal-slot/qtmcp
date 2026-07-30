// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPRESULT_H
#define QMCPRESULT_H

#include <QtCore/QJsonObject>
#include <QtMcpCommon/qmcpgadget.h>
#include <QtMcpCommon/qmcpresultmeta.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpResult
    \inmodule QtMcpCommon
*/
class Q_MCPCOMMON_EXPORT QMcpResult : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpResult::_meta
        \brief This result property is reserved by the protocol to allow clients and servers to attach additional metadata to their responses.
    */
    Q_PROPERTY(QMcpResultMeta _meta READ meta WRITE setMeta)
    Q_PROPERTY(QJsonObject additionalProperties READ additionalProperties WRITE setAdditionalProperties)

    /*!
        \property QMcpResult::resultType
        \brief The type of the result: "complete" for ordinary results,
        "input_required" for multi round-trip interim results.

        Required on the wire since MCP 2026-07-28; earlier revisions never
        serialize it. A missing field on an incoming result means "complete".
        \since MCP 2026-07-28
    */
    Q_PROPERTY(QString resultType READ resultType WRITE setResultType REQUIRED)

public:
    QMcpResult() : QMcpGadget(new Private) {}
protected:
    QMcpResult(Private *d) : QMcpGadget(d) {}
public:

    QMcpResultMeta meta() const {
        return d<Private>()->_meta;
    }

    void setMeta(const QMcpResultMeta &meta) {
        if (this->meta() == meta) return;
        d<Private>()->_meta = meta;
    }

    QJsonObject additionalProperties() const {
        return d<Private>()->additionalProperties;
    }

    void setAdditionalProperties(const QJsonObject &props) {
        if (this->additionalProperties() == props) return;
        d<Private>()->additionalProperties = props;
    }

    QString resultType() const {
        return d<Private>()->resultType;
    }

    void setResultType(const QString &resultType) {
        if (this->resultType() == resultType) return;
        d<Private>()->resultType = resultType;
    }

    bool fromJsonObject(const QJsonObject &object, QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) override {
        // Even on 2026-07-28, where the field is mandatory, tolerate results
        // of servers that omit it: the spec's compatibility rule is to treat
        // an absent resultType as "complete".
        if (protocolVersion >= QtMcp::ProtocolVersion::v2026_07_28 && !object.contains("resultType"_L1)) {
            auto copy = object;
            copy.insert("resultType"_L1, "complete"_L1);
            return QMcpGadget::fromJsonObject(copy, protocolVersion);
        }
        return QMcpGadget::fromJsonObject(object, protocolVersion);
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "resultType")
            return protocolVersion >= QtMcp::ProtocolVersion::v2026_07_28;
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

    struct Private : public QMcpGadget::Private {
        QMcpResultMeta _meta;
        QJsonObject additionalProperties;
        QString resultType = QStringLiteral("complete");

        Private *clone() const override { return new Private(*this); }
    };
};

QT_END_NAMESPACE

#endif // QMCPRESULT_H
