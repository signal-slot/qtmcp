// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPELICITREQUESTPARAMS_H
#define QMCPELICITREQUESTPARAMS_H

#include <QtCore/QJsonObject>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtMcpCommon/qmcpelicitrequestparamsrequestedschema.h>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpElicitRequestParams
    \inmodule QtMcpCommon
    \brief The parameters of a request to elicit additional information from the user via the client.

    The 2025-11-25 revision splits these parameters into the two variants
    ElicitRequestFormParams and ElicitRequestURLParams. Instead of a union this
    class keeps both in one flat type and tells them apart by mode, the member
    the specification uses as the discriminator anyway:

    \list
    \li mode is "form" (the default, and the only mode before 2025-11-25): the
        client asks the user for the values described by requestedSchema and
        returns them in QMcpElicitResult::content. url and elicitationId are
        unused.
    \li mode is "url": the client sends the user to url instead and the server
        refers to that interaction by elicitationId, which also identifies it in
        QMcpElicitationCompleteNotification. requestedSchema is unused and
        QMcpElicitResult carries no content.
    \endlist

    Which members are serialized therefore depends on mode: requestedSchema is
    dropped in "url" mode, and mode, url and elicitationId are dropped for
    protocol revisions older than 2025-11-25.
*/
class Q_MCPCOMMON_EXPORT QMcpElicitRequestParams : public QMcpGadget
{
    Q_GADGET

    /*!
        \property QMcpElicitRequestParams::elicitationId
        \brief The ID of the elicitation, unique within the context of the server.

        Only used when mode is "url". The client MUST treat this ID as an opaque
        value.
        \since MCP 2025-11-25
    */
    Q_PROPERTY(QString elicitationId READ elicitationId WRITE setElicitationId)

    /*!
        \property QMcpElicitRequestParams::message
        \brief The message to present to the user.

        In "form" mode it describes what information is being requested, in
        "url" mode why the interaction is needed.
    */
    Q_PROPERTY(QString message READ message WRITE setMessage REQUIRED)

    /*!
        \property QMcpElicitRequestParams::mode
        \brief The elicitation mode, either "form" or "url".

        The default value is "form", the only mode the 2025-06-18 revision
        knows. See the class documentation for what each mode means.
        \since MCP 2025-11-25
    */
    Q_PROPERTY(QString mode READ mode WRITE setMode)

    /*!
        \property QMcpElicitRequestParams::requestedSchema
        \brief A restricted subset of JSON Schema describing the requested values.

        Only used when mode is "form".
    */
    Q_PROPERTY(QMcpElicitRequestParamsRequestedSchema requestedSchema READ requestedSchema WRITE setRequestedSchema REQUIRED)

    /*!
        \property QMcpElicitRequestParams::url
        \brief The URL the user should navigate to.

        Only used when mode is "url".
        \since MCP 2025-11-25
    */
    Q_PROPERTY(QUrl url READ url WRITE setUrl)

public:
    QMcpElicitRequestParams() : QMcpGadget(new Private) {}

    QString elicitationId() const {
        return d<Private>()->elicitationId;
    }

    void setElicitationId(const QString &elicitationId) {
        if (this->elicitationId() == elicitationId) return;
        d<Private>()->elicitationId = elicitationId;
    }

    QString message() const {
        return d<Private>()->message;
    }

    void setMessage(const QString &message) {
        if (this->message() == message) return;
        d<Private>()->message = message;
    }

    QString mode() const {
        return d<Private>()->mode;
    }

    void setMode(const QString &mode) {
        if (this->mode() == mode) return;
        d<Private>()->mode = mode;
    }

    QMcpElicitRequestParamsRequestedSchema requestedSchema() const {
        return d<Private>()->requestedSchema;
    }

    void setRequestedSchema(const QMcpElicitRequestParamsRequestedSchema &requestedSchema) {
        if (this->requestedSchema() == requestedSchema) return;
        d<Private>()->requestedSchema = requestedSchema;
    }

    QUrl url() const {
        return d<Private>()->url;
    }

    void setUrl(const QUrl &url) {
        if (this->url() == url) return;
        d<Private>()->url = url;
    }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    bool fromJsonObject(const QJsonObject &object, QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest) override {
        // mode decides which of the two variants the object is, and with it
        // which members isPropertyAvailable() accepts. It therefore has to be
        // known before the base implementation walks the properties, whose order
        // this class must not depend on.
        if (protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25) {
            const auto mode = object.value("mode"_L1).toString();
            setMode(mode.isEmpty() ? formMode() : mode);
        }
        return QMcpGadget::fromJsonObject(object, protocolVersion);
    }

protected:
    bool isPropertyAvailable(QByteArrayView name, QtMcp::ProtocolVersion protocolVersion) const override {
        if (name == "elicitationId" || name == "mode" || name == "url")
            return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25;
        if (name == "requestedSchema")
            return !isUrlMode(protocolVersion);
        return QMcpGadget::isPropertyAvailable(name, protocolVersion);
    }

private:
    static QString formMode() { return QStringLiteral("form"); }
    static QString urlMode() { return QStringLiteral("url"); }

    // The URL variant only exists since 2025-11-25; older revisions always
    // expect the form members, whatever mode happens to say.
    bool isUrlMode(QtMcp::ProtocolVersion protocolVersion) const {
        return protocolVersion >= QtMcp::ProtocolVersion::v2025_11_25 && mode() == urlMode();
    }

    struct Private : public QMcpGadget::Private {
        QString elicitationId;
        QString message;
        QString mode = QStringLiteral("form");
        QMcpElicitRequestParamsRequestedSchema requestedSchema;
        QUrl url;

        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpElicitRequestParams)

QT_END_NAMESPACE

#endif // QMCPELICITREQUESTPARAMS_H
