// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserversession.h"
#include "qmcpserver.h"
#include <QtCore/QMultiHash>

QT_BEGIN_NAMESPACE

class QMcpServerSession::Private
{
public:
    Private(const QUuid &id)
        : sessionId(id)
        , initialized(false)
    {}

    QUuid sessionId;
    bool initialized;
    QList<QMcpResourceTemplate> resourceTemplates;
    QList<QPair<QMcpResource, QMcpReadResourceResultContents>> resources;
    QList<QPair<QMcpPrompt, QMcpPromptMessage>> prompts;
    QList<QMcpRoot> roots;
    QMultiHash<QUrl, QUrl> subscriptions;
};

QMcpServerSession::QMcpServerSession(const QUuid &sessionId, QMcpServer *parent)
    : QObject(parent)
    , d(new Private(sessionId))
{}

QMcpServerSession::~QMcpServerSession() = default;

QUuid QMcpServerSession::sessionId() const
{
    return d->sessionId;
}

bool QMcpServerSession::isInitialized() const
{
    return d->initialized;
}

void QMcpServerSession::setInitialized(bool initialized)
{
    if (d->initialized == initialized) return;
    d->initialized = initialized;
    emit initializedChanged(initialized);
}

void QMcpServerSession::appendResourceTemplate(const QMcpResourceTemplate &resourceTemplate)
{
    d->resourceTemplates.append(resourceTemplate);
}

void QMcpServerSession::insertResourceTemplate(int index, const QMcpResourceTemplate &resourceTemplate)
{
    d->resourceTemplates.insert(index, resourceTemplate);
}

void QMcpServerSession::replaceResourceTemplate(int index, const QMcpResourceTemplate resourceTemplate)
{
    d->resourceTemplates[index] = resourceTemplate;
}

void QMcpServerSession::removeResourceTemplateAt(int index)
{
    d->resourceTemplates.removeAt(index);
}

void QMcpServerSession::appendResource(const QMcpResource &resource, const QMcpReadResourceResultContents &content)
{
    d->resources.append(qMakePair(resource, content));
}

void QMcpServerSession::insertResource(int index, const QMcpResource &resource, const QMcpReadResourceResultContents &content)
{
    d->resources.insert(index, qMakePair(resource, content));
}

void QMcpServerSession::replaceResource(int index, const QMcpResource resource, const QMcpReadResourceResultContents &content)
{
    d->resources[index] = qMakePair(resource, content);
    emit resourceUpdated(resource);
}

void QMcpServerSession::removeResourceAt(int index)
{
    d->resources.removeAt(index);
}

QList<QMcpResourceTemplate> QMcpServerSession::resourceTemplates() const
{
    return d->resourceTemplates;
}

QList<QMcpResource> QMcpServerSession::resources() const
{
    QList<QMcpResource> ret;
    ret.reserve(d->resources.count());
    for (const auto &pair : std::as_const(d->resources))
        ret.append(pair.first);
    return ret;
}

QList<QMcpReadResourceResultContents> QMcpServerSession::contents(const QUrl &uri) const
{
    QList<QMcpReadResourceResultContents> ret;
    for (const auto &pair : std::as_const(d->resources))
        if (pair.first.uri() == uri)
            ret.append(pair.second);
    return ret;
}

void QMcpServerSession::appendPrompt(const QMcpPrompt &prompt, const QMcpPromptMessage &message)
{
    d->prompts.append(qMakePair(prompt, message));
}

void QMcpServerSession::insertPrompt(int index, const QMcpPrompt &prompt, const QMcpPromptMessage &message)
{
    d->prompts.insert(index, qMakePair(prompt, message));
}

void QMcpServerSession::replacePrompt(int index, const QMcpPrompt prompt, const QMcpPromptMessage &message)
{
    d->prompts[index] = qMakePair(prompt, message);
}

void QMcpServerSession::removePromptAt(int index)
{
    d->prompts.removeAt(index);
}

QList<QMcpPrompt> QMcpServerSession::prompts(QString *cursor) const
{
    Q_UNUSED(cursor); // TODO
    QList<QMcpPrompt> ret;
    for (const auto &pair : std::as_const(d->prompts))
        ret.append(pair.first);
    return ret;
}

QList<QMcpPromptMessage> QMcpServerSession::messages(const QString &name) const
{
    QList<QMcpPromptMessage> ret;
    for (const auto &pair : std::as_const(d->prompts))
        if (pair.first.name() == name)
            ret.append(pair.second);
    return ret;
}

namespace {
template<class T>
T callMethod(QObject *object, const QMetaMethod *method, const QVariantList &args)
{
    T result;
    QGenericReturnArgument ret(method->returnMetaType().name(), &result);
    switch (method->parameterCount()) {
    case 0:
        method->invoke(object,
                       ret
                       );
        break;
    case 1:
        method->invoke(object,
                       ret,
                       QGenericArgument(args.at(0).typeName(), args.at(0).constData())
                       );
        break;
    case 2:
        method->invoke(object,
                       ret,
                       QGenericArgument(args.at(0).typeName(), args.at(0).constData()),
                       QGenericArgument(args.at(1).typeName(), args.at(1).constData())
                       );
        break;
    case 3:
        method->invoke(object,
                       ret,
                       QGenericArgument(args.at(0).typeName(), args.at(0).constData()),
                       QGenericArgument(args.at(1).typeName(), args.at(1).constData()),
                       QGenericArgument(args.at(2).typeName(), args.at(2).constData())
                       );
        break;
    default:
        qFatal() << "callMethod: too many parameters, or not implemented in switch.";
    }
    return result;
}
}

QList<QMcpCallToolResultContent> QMcpServerSession::callTool(const QString &name, const QJsonObject &params, bool *ok)
{
    QList<QMcpCallToolResultContent> ret;
    bool found = false;
    const auto *mo = parent()->metaObject();
    for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
        const auto mm = mo->method(i);
        if (mm.name() != name.toUtf8())
            continue;

        auto checkParams = [](const QStringList &a, QByteArrayList b) {
            QByteArrayList c;
            for (const auto &s : a)
                c.append(s.toUtf8());
            std::sort(b.begin(), b.end(), std::less<QByteArray>());
            std::sort(c.begin(), c.end(), std::less<QByteArray>());
            return b == c;
        };

        const auto names = mm.parameterNames();
        switch (mm.parameterCount() - params.size()) {
        case 0: // exact match
            if (!checkParams(params.keys(), names))
                continue;
            break;
        case 1: // may contain session
            if (!checkParams(params.keys() << "sessionId"_L1, names))
                continue;
            break;
        default:
            continue;
        }

        QVariantList convertedArgs;
        convertedArgs.reserve(mm.parameterCount());

        const auto types = mm.parameterTypes();
        for (int j = 0; j < mm.parameterCount(); j++) {
            const auto type = types.at(j);
            const auto name = QString::fromUtf8(names.at(j));

            const auto metaType = QMetaType::fromName(type);
            switch (metaType.id()) {
            case QMetaType::UnknownType:
                qWarning() << "Unknown or unsupported type:" << type;
                break;
            case QMetaType::QUuid:
                convertedArgs.append(d->sessionId);
                continue;
            default: {
                auto value = params.value(name).toVariant();
                if (!value.convert(mm.parameterMetaType(j))) {
                    qWarning() << "Failed to convert JSON value to type:" << type;
                    break;
                }
                convertedArgs.append(value);
                break; }
            }

        }

        if (convertedArgs.count() != mm.parameterCount())
            continue;

        switch (mm.returnMetaType().id()) {
        case QMetaType::Void: {
            switch (mm.parameterCount()) {
            case 0:
                mm.invoke(parent(),
                          Qt::DirectConnection
                          );
                break;
            case 1:
                mm.invoke(parent(),
                          Qt::DirectConnection,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData())
                          );
                break;
            case 2:
                mm.invoke(parent(),
                          Qt::DirectConnection,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                          QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData())
                          );
                break;
            case 3:
                mm.invoke(parent(),
                          Qt::DirectConnection,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                          QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData()),
                          QGenericArgument(convertedArgs[2].typeName(), convertedArgs[2].constData())
                          );
                break;
            default:
                qFatal() << "invokeMethodWithJson: too many parameters, or not implemented in switch.";
            }
            found = true;
            break; }
        case QMetaType::QString: {
            found = true;
            QString text = callMethod<QString>(parent(), &mm, convertedArgs);
            ret.append(QMcpTextContent(text));
            break; }
#ifdef QT_GUI_LIB
        case QMetaType::QImage: {
            found = true;
            QImage image = callMethod<QImage>(parent(), &mm, convertedArgs);
            ret.append(QMcpImageContent(image));
            break; }
#endif // QT_GUI_LIB
        default:
            qFatal() << mm.returnMetaType() << "not supported yet";
        }
        break;
    }
    if (ok)
        *ok = found;
    if (!found)
        qWarning() << name << "not found for " << params;
    return ret;
}

QList<QMcpRoot> QMcpServerSession::roots() const
{
    return d->roots;
}

void QMcpServerSession::setRoots(const QList<QMcpRoot> &roots)
{
    if (d->roots == roots) return;
    d->roots = roots;
    emit rootsChanged(roots);
}

void QMcpServerSession::subscribe(const QUrl &uri)
{
    d->subscriptions.insert(uri, uri);
}

void QMcpServerSession::unsubscribe(const QUrl &uri)
{
    d->subscriptions.remove(uri);
}

bool QMcpServerSession::isSubscribed(const QUrl &uri) const
{
    return d->subscriptions.contains(uri);
}

QT_END_NAMESPACE
