// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPTASKSEXTENSIONCAPABILITY_H
#define QMCPTASKSEXTENSIONCAPABILITY_H

#include <QtCore/QString>
#include <QtMcpCommon/qmcpgadget.h>

QT_BEGIN_NAMESPACE

/*! \class QMcpTasksExtensionCapability
    \inmodule QtMcpCommon
    \brief The settings object announcing support for the tasks extension.

    This type belongs to the \c io.modelcontextprotocol/tasks extension rather
    than to a protocol revision, so it is not version gated. It has no core
    counterpart: MCP 2025-11-25 negotiated tasks as part of the protocol itself.

    The extension defines no settings, and the schema spells that out as an
    object whose \c additionalProperties are \c {"not": {}}, i.e. no member is
    permitted at all. The type therefore carries no properties and serializes to
    an empty object; its presence under \l identifier() is the whole signal.

    A client announces the extension per request, in the client capabilities it
    attaches to \c _meta; a server announces it in the capabilities it returns
    from server/discover. Both spellings are keyed by \l identifier():

    \code
    QMcpClientCapabilities capabilities;
    capabilities.setExtensions({
        { QMcpTasksExtensionCapability::identifier(),
          QMcpTasksExtensionCapability().toJsonObject() }
    });
    \endcode

    A server must not answer with QMcpExtCreateTaskResult unless the client
    declared this capability on the request in question.

    \sa QMcpClientCapabilities::extensions, QMcpServerCapabilities::extensions,
        QMcpExtCreateTaskResult
*/
class Q_MCPCOMMON_EXPORT QMcpTasksExtensionCapability : public QMcpGadget
{
    Q_GADGET

public:
    QMcpTasksExtensionCapability() : QMcpGadget(new Private) {}

    /*!
        Returns the identifier the extension is registered under, that is the key
        this capability is stored at in QMcpClientCapabilities::extensions and
        QMcpServerCapabilities::extensions.

        This is not a property: the schema forbids any member on the capability
        object, so the identifier must not end up in the serialized form.
    */
    static QString identifier() { return "io.modelcontextprotocol/tasks"_L1; }

    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

private:
    struct Private : public QMcpGadget::Private {
        Private *clone() const override { return new Private(*this); }
    };
};

Q_DECLARE_SHARED(QMcpTasksExtensionCapability)

QT_END_NAMESPACE

#endif // QMCPTASKSEXTENSIONCAPABILITY_H
