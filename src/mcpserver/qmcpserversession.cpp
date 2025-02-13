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
    QList<QPair<QMcpTool, QObject *>> tools;
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
    emit resourceListChanged();
}

void QMcpServerSession::insertResource(int index, const QMcpResource &resource, const QMcpReadResourceResultContents &content)
{
    d->resources.insert(index, qMakePair(resource, content));
    emit resourceListChanged();
}

void QMcpServerSession::replaceResource(int index, const QMcpResource resource, const QMcpReadResourceResultContents &content)
{
    d->resources[index] = qMakePair(resource, content);
    emit resourceUpdated(resource);
}

void QMcpServerSession::removeResourceAt(int index)
{
    d->resources.removeAt(index);
    emit resourceListChanged();
}

QList<QMcpResourceTemplate> QMcpServerSession::resourceTemplates() const
{
    return d->resourceTemplates;
}

QList<QMcpResource> QMcpServerSession::resources(QString *cursor) const
{
    QList<QMcpResource> ret;
    const int pageSize = 50; // Default page size
    
    // Collect all resources first
    QList<QMcpResource> allResources;
    allResources.reserve(d->resources.count());
    for (const auto &pair : std::as_const(d->resources))
        allResources.append(pair.first);
    
    // Start from the cursor position if provided
    int startIndex = cursor && !cursor->isEmpty() ? cursor->toInt() : 0;
    if (startIndex < 0 || startIndex >= allResources.count())
        startIndex = 0;
    
    // Get one page of results
    int endIndex = qMin(startIndex + pageSize, allResources.count());
    ret.reserve(endIndex - startIndex);
    
    for (int i = startIndex; i < endIndex; ++i)
        ret.append(allResources.at(i));
    
    // Update cursor for next page
    if (cursor) {
        if (endIndex < allResources.count())
            *cursor = QString::number(endIndex);
        else
            cursor->clear(); // No more pages
    }
    
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
    emit promptListChanged();
}

void QMcpServerSession::insertPrompt(int index, const QMcpPrompt &prompt, const QMcpPromptMessage &message)
{
    d->prompts.insert(index, qMakePair(prompt, message));
    emit promptListChanged();
}

void QMcpServerSession::replacePrompt(int index, const QMcpPrompt prompt, const QMcpPromptMessage &message)
{
    d->prompts[index] = qMakePair(prompt, message);
    emit promptListChanged();
}

void QMcpServerSession::removePromptAt(int index)
{
    d->prompts.removeAt(index);
    emit promptListChanged();
}

QList<QMcpPrompt> QMcpServerSession::prompts(QString *cursor) const
{
    QList<QMcpPrompt> ret;
    const int pageSize = 50; // Default page size
    
    // Collect all prompts first
    QList<QMcpPrompt> allPrompts;
    allPrompts.reserve(d->prompts.count());
    for (const auto &pair : std::as_const(d->prompts))
        allPrompts.append(pair.first);
    
    // Start from the cursor position if provided
    int startIndex = cursor && !cursor->isEmpty() ? cursor->toInt() : 0;
    if (startIndex < 0 || startIndex >= allPrompts.count())
        startIndex = 0;
    
    // Get one page of results
    int endIndex = qMin(startIndex + pageSize, allPrompts.count());
    ret.reserve(endIndex - startIndex);
    
    for (int i = startIndex; i < endIndex; ++i)
        ret.append(allPrompts.at(i));
    
    // Update cursor for next page
    if (cursor) {
        if (endIndex < allPrompts.count())
            *cursor = QString::number(endIndex);
        else
            cursor->clear(); // No more pages
    }
    
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

void QMcpServerSession::registerToolSet(QObject *toolSet, const QHash<QString, QString> &descriptions)
{
    const auto *mo = toolSet->metaObject();

    QString prefix = toolSet->objectName();
    if (!prefix.isEmpty())
        prefix.prepend('/'_L1);

    bool changed = false;
    for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
        const auto mm = mo->method(i);
        QMcpTool tool;
        const auto name = QString::fromUtf8(mm.name());
        qDebug() << name;
        tool.setName(prefix + name);
        if (descriptions.contains(name)) {
            tool.setDescription(descriptions.value(name));
        }
        QMcpToolInputSchema inputSchema;
        auto required = inputSchema.required();
        auto properties = inputSchema.properties();
        const auto types = mm.parameterTypes();
        const auto names = mm.parameterNames();
        for (int j = 0; j < mm.parameterCount(); j++) {
            const auto type = QString::fromUtf8(types.at(j));
            const auto name = QString::fromUtf8(names.at(j));
            // message
            QHash<QString, QString> mcpTypes {
                                             { "QString", "string" },
                                             { "int", "number" },
                                             };
            QSet<QString> internalTypes { "QUuid"_L1 };
            QJsonObject object;
            if (mcpTypes.contains(type))
                object.insert("type"_L1, mcpTypes.value(type));
            else if (internalTypes.contains(type))
                continue;
            else
                qWarning() << "Unknown type" << type;

            if (descriptions.contains("%1/%2"_L1.arg(tool.name(), name))) {
                object.insert("description"_L1, descriptions.value("%1/%2"_L1.arg(tool.name(), name)));
            }
            properties.insert(name, object);
            required.append(name);
        }
        inputSchema.setProperties(properties);
        inputSchema.setRequired(required);
        tool.setInputSchema(inputSchema);
        d->tools.append(std::make_pair(tool, toolSet));
        changed = true;
    }
    if (changed)
        emit toolListChanged();
}

void QMcpServerSession::unregisterToolSet(const QObject *toolSet)
{
    bool changed = false;
    for (int i = d->tools.length() - 1; i >= 0; i--) {
        if (d->tools.at(i).second == toolSet) {
            d->tools.removeAt(i);
            changed = true;
        }
    }
    if (changed)
        emit toolListChanged();
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

QList<QMcpTool> QMcpServerSession::tools(QString *cursor) const
{
    Q_UNUSED(cursor);
    QList<QMcpTool> ret;
    for (const auto &pair : std::as_const(d->tools))
        ret.append(pair.first);
    return ret;
}

QList<QMcpCallToolResultContent> QMcpServerSession::callTool(const QString &name, const QJsonObject &params, bool *ok)
{
    bool found = false;
    QList<QMcpCallToolResultContent> ret;
    for (const auto &pair : std::as_const(d->tools)) {
        const auto tool = pair.first;
        // check name first
        if (tool.name() != name)
            continue;

        QString prefix = pair.second->objectName();
        if (!prefix.isEmpty())
            prefix.prepend('/'_L1);

        // check params next
        const auto *mo = pair.second->metaObject();
        for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
            const auto mm = mo->method(i);
            if (prefix + QString::fromUtf8(mm.name()) != name)
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
                    mm.invoke(pair.second,
                              Qt::DirectConnection
                              );
                    break;
                case 1:
                    mm.invoke(pair.second,
                              Qt::DirectConnection,
                              QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData())
                              );
                    break;
                case 2:
                    mm.invoke(pair.second,
                              Qt::DirectConnection,
                              QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                              QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData())
                              );
                    break;
                case 3:
                    mm.invoke(pair.second,
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
                QString text = callMethod<QString>(pair.second, &mm, convertedArgs);
                ret.append(QMcpTextContent(text));
                break; }
#ifdef QT_GUI_LIB
            case QMetaType::QImage: {
                found = true;
                QImage image = callMethod<QImage>(pair.second, &mm, convertedArgs);
                ret.append(QMcpImageContent(image));
                break; }
#endif // QT_GUI_LIB
            default:
                qFatal() << mm.returnMetaType() << "not supported yet";
            }

            break;
        }
    }

    if (ok)
        *ok = found;
    if (!found)
        qWarning() << name << "not found for " << params;
    return ret;
}

QList<QMcpRoot> QMcpServerSession::roots(QString *cursor) const
{
    QList<QMcpRoot> ret;
    const int pageSize = 50; // Default page size
    
    // Start from the cursor position if provided
    int startIndex = cursor && !cursor->isEmpty() ? cursor->toInt() : 0;
    if (startIndex < 0 || startIndex >= d->roots.count())
        startIndex = 0;
    
    // Get one page of results
    int endIndex = qMin(startIndex + pageSize, d->roots.count());
    ret.reserve(endIndex - startIndex);
    
    for (int i = startIndex; i < endIndex; ++i)
        ret.append(d->roots.at(i));
    
    // Update cursor for next page
    if (cursor) {
        if (endIndex < d->roots.count())
            *cursor = QString::number(endIndex);
        else
            cursor->clear(); // No more pages
    }
    
    return ret;
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
