// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserversession.h"
#include "qmcpserver.h"
#include <QtCore/QFutureWatcher>
#include <QtCore/QMultiHash>
#include <QtCore/QPromise>
#include <QtCore/QTimer>
#ifdef QT_GUI_LIB
#include <QtGui/QAction>
#endif
#include <QtMcpCommon/QMcpCreateMessageRequest>
#include <QtMcpCommon/QMcpProgressNotification>

QT_BEGIN_NAMESPACE

class QMcpServerSession::Private
{
public:
    Private(const QUuid &id, QMcpServerSession *parent);

private:
    QMcpServerSession *q;

public:
    QUuid sessionId;
    bool initialized = false;
    QtMcp::ProtocolVersion protocolVersion = QtMcp::ProtocolVersion::Latest; // Default to latest version
    QList<QMcpResourceTemplate> resourceTemplates;
    QList<QPair<QMcpResource, QMcpReadResourceResultContents>> resources;
    QList<QPair<QMcpPrompt, QMcpPromptMessage>> prompts;
    QList<QPair<QMcpTool, QObject *>> tools;
#ifdef QT_GUI_LIB
    QList<QPair<QMcpTool, QAction *>> actions;
#endif
    QList<QMcpRoot> roots;
    QMultiHash<QUrl, QUrl> subscriptions;

    QTimer notifyResourceListChanged;
    QTimer notifyPromptListChanged;
    QTimer notifyToolListChanged;
};

QMcpServerSession::Private::Private(const QUuid &id, QMcpServerSession *parent)
    : q(parent)
    , sessionId(id)
{
    notifyResourceListChanged.setInterval(0);
    notifyResourceListChanged.setSingleShot(true);
    connect(&notifyResourceListChanged, &QTimer::timeout, q, &QMcpServerSession::resourceListChanged);

    notifyPromptListChanged.setInterval(0);
    notifyPromptListChanged.setSingleShot(true);
    connect(&notifyPromptListChanged, &QTimer::timeout, q, &QMcpServerSession::promptListChanged);

    notifyToolListChanged.setInterval(0);
    notifyToolListChanged.setSingleShot(true);
    connect(&notifyToolListChanged, &QTimer::timeout, q, &QMcpServerSession::toolListChanged);
}

QMcpServerSession::QMcpServerSession(const QUuid &sessionId, QMcpServer *parent)
    : QObject(parent)
    , d(new Private(sessionId, this))
{}

QMcpServerSession::~QMcpServerSession() = default;

QUuid QMcpServerSession::sessionId() const
{
    return d->sessionId;
}

QtMcp::ProtocolVersion QMcpServerSession::protocolVersion() const
{
    return d->protocolVersion;
}

void QMcpServerSession::setProtocolVersion(QtMcp::ProtocolVersion protocolVersion)
{
    if (d->protocolVersion == protocolVersion)
        return;

    d->protocolVersion = protocolVersion;
}

void QMcpServerSession::setProtocolVersion(const QString &protocolVersionStr)
{
    // Convert the string to enum using the utility function
    const QtMcp::ProtocolVersion version = QtMcp::stringToProtocolVersion(protocolVersionStr);

    // Use the enum-based method
    setProtocolVersion(version);
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
    d->resourceTemplates.replace(index, resourceTemplate);
}

void QMcpServerSession::removeResourceTemplateAt(int index)
{
    d->resourceTemplates.removeAt(index);
}

void QMcpServerSession::appendResource(const QMcpResource &resource, const QMcpReadResourceResultContents &content)
{
    d->resources.append(qMakePair(resource, content));
    d->notifyResourceListChanged.start();
}

void QMcpServerSession::insertResource(int index, const QMcpResource &resource, const QMcpReadResourceResultContents &content)
{
    d->resources.insert(index, qMakePair(resource, content));
    d->notifyResourceListChanged.start();
}

void QMcpServerSession::replaceResource(const QUrl &uri, const QMcpResource resource, const QMcpReadResourceResultContents &content)
{
    for (int i = 0; i < d->resources.count(); ++i) {
        if (d->resources.at(i).first.uri() == uri) {
            d->resources.replace(i, qMakePair(resource, content));
            emit resourceUpdated(resource);
            break;
        }
    }
}

void QMcpServerSession::replaceResource(int index, const QMcpResource resource, const QMcpReadResourceResultContents &content)
{
    d->resources.replace(index, qMakePair(resource, content));
    emit resourceUpdated(resource);
}

void QMcpServerSession::removeResource(const QUrl &uri)
{
    for (int i = 0; i < d->resources.count(); ++i) {
        if (d->resources.at(i).first.uri() == uri) {
            d->resources.removeAt(i);
            d->notifyResourceListChanged.start();
            break;
        }
    }
}

void QMcpServerSession::removeResourceAt(int index)
{
    d->resources.removeAt(index);
    d->notifyResourceListChanged.start();
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
    qDebug() << Q_FUNC_INFO << __LINE__ << uri;
    QList<QMcpReadResourceResultContents> ret;
    for (const auto &pair : std::as_const(d->resources)) {
        qDebug() << Q_FUNC_INFO << __LINE__ << pair.first.name() << pair.first.size() << pair.first.uri();
        if (pair.first.uri() == uri)
            ret.append(pair.second);
    }
    return ret;
}

void QMcpServerSession::appendPrompt(const QMcpPrompt &prompt, const QMcpPromptMessage &message)
{
    d->prompts.append(qMakePair(prompt, message));
    d->notifyPromptListChanged.start();
}

void QMcpServerSession::insertPrompt(int index, const QMcpPrompt &prompt, const QMcpPromptMessage &message)
{
    d->prompts.insert(index, qMakePair(prompt, message));
    d->notifyPromptListChanged.start();
}

void QMcpServerSession::replacePrompt(int index, const QMcpPrompt prompt, const QMcpPromptMessage &message)
{
    d->prompts.replace(index, qMakePair(prompt, message));
    d->notifyPromptListChanged.start();
}

void QMcpServerSession::removePromptAt(int index)
{
    d->prompts.removeAt(index);
    d->notifyPromptListChanged.start();
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
        prefix.append('/'_L1);

    // Collect methods grouped by name, picking the overload with the most parameters
    // and tracking the minimum parameter count to determine which params are required.
    // Qt MOC generates multiple overloads for methods with default parameter values.
    struct MethodInfo {
        QMetaMethod method;
        int minParamCount;
    };
    QHash<QString, MethodInfo> methodMap;

    for (int i = mo->methodOffset(); i < mo->methodCount(); i++) {
        const auto mm = mo->method(i);
        if (mm.access() != QMetaMethod::Public)
            continue;
        if (mm.methodType() == QMetaMethod::Signal || mm.methodType() == QMetaMethod::Constructor)
            continue;
        const auto name = QString::fromUtf8(mm.name());
        if (!methodMap.contains(name)) {
            methodMap.insert(name, { mm, mm.parameterCount() });
        } else {
            auto &info = methodMap[name];
            if (mm.parameterCount() > info.method.parameterCount()) {
                info.minParamCount = qMin(info.minParamCount, info.method.parameterCount());
                info.method = mm;
            } else {
                info.minParamCount = qMin(info.minParamCount, mm.parameterCount());
            }
        }
    }

    bool changed = false;
    for (auto it = methodMap.cbegin(); it != methodMap.cend(); ++it) {
        const auto &name = it.key();
        const auto &mm = it.value().method;
        const int minParams = it.value().minParamCount;

        QMcpTool tool;
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
                                             { "bool", "boolean" },
                                             { "int", "integer" },
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
            if (j < minParams)
                required.append(name);
        }
        inputSchema.setProperties(properties);
        inputSchema.setRequired(required);
        tool.setInputSchema(inputSchema);
        d->tools.append(std::make_pair(tool, toolSet));
        changed = true;
    }
    if (changed)
        d->notifyToolListChanged.start();
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
        d->notifyToolListChanged.start();
}

#ifdef QT_GUI_LIB
void QMcpServerSession::registerTool(QAction *action, const QString &name)
{
    QMcpTool tool;
    tool.setName(name);
    tool.setDescription(action->toolTip());
    d->actions.append(std::make_pair(tool, action));
    d->notifyToolListChanged.start();
}

void QMcpServerSession::unregisterTool(const QAction *action)
{
    for (int i = d->actions.length() - 1; i >= 0; i--) {
        if (d->actions.at(i).second == action) {
            d->actions.removeAt(i);
            d->notifyToolListChanged.start();
            return;
        }
    }
}
#endif

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
    case 4:
        method->invoke(object,
                       ret,
                       QGenericArgument(args.at(0).typeName(), args.at(0).constData()),
                       QGenericArgument(args.at(1).typeName(), args.at(1).constData()),
                       QGenericArgument(args.at(2).typeName(), args.at(2).constData()),
                       QGenericArgument(args.at(3).typeName(), args.at(3).constData())
                       );
        break;
    case 5:
        method->invoke(object,
                       ret,
                       QGenericArgument(args.at(0).typeName(), args.at(0).constData()),
                       QGenericArgument(args.at(1).typeName(), args.at(1).constData()),
                       QGenericArgument(args.at(2).typeName(), args.at(2).constData()),
                       QGenericArgument(args.at(3).typeName(), args.at(3).constData()),
                       QGenericArgument(args.at(4).typeName(), args.at(4).constData())
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
#ifdef QT_GUI_LIB
    for (const auto &pair : std::as_const(d->actions))
        ret.append(pair.first);
#endif
    std::sort(ret.begin(), ret.end(), [](const QMcpTool &tool1, const QMcpTool &tool2) {
        return tool1.name() < tool2.name();
    });
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
            prefix.append('/'_L1);

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
                case 4:
                    mm.invoke(pair.second,
                              Qt::DirectConnection,
                              QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                              QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData()),
                              QGenericArgument(convertedArgs[2].typeName(), convertedArgs[2].constData()),
                              QGenericArgument(convertedArgs[3].typeName(), convertedArgs[3].constData())
                              );
                    break;
                case 5:
                    mm.invoke(pair.second,
                              Qt::DirectConnection,
                              QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                              QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData()),
                              QGenericArgument(convertedArgs[2].typeName(), convertedArgs[2].constData()),
                              QGenericArgument(convertedArgs[3].typeName(), convertedArgs[3].constData()),
                              QGenericArgument(convertedArgs[4].typeName(), convertedArgs[4].constData())
                              );
                    break;
                default:
                    qFatal() << "invokeMethodWithJson: too many parameters, or not implemented in switch.";
                }
                found = true;
                break; }
            case QMetaType::Bool: {
                found = true;
                bool boolean = callMethod<bool>(pair.second, &mm, convertedArgs);
                ret.append(QMcpTextContent(boolean ? "true"_L1 : "false"_L1));
                break; }
            case QMetaType::QString: {
                found = true;
                QString text = callMethod<QString>(pair.second, &mm, convertedArgs);
                ret.append(QMcpTextContent(text));
                break; }
            case QMetaType::QStringList: {
                found = true;
                const QStringList texts = callMethod<QStringList>(pair.second, &mm, convertedArgs);
                for (const auto &text : texts)
                    ret.append(QMcpTextContent(text));
                break; }
#ifdef QT_GUI_LIB
            case QMetaType::QImage: {
                found = true;
                QImage image = callMethod<QImage>(pair.second, &mm, convertedArgs);
                if (image.isNull()) {
                    ret.append(QMcpTextContent("image is null"_L1));
                } else {
                    ret.append(QMcpImageContent(image));
                }
                break; }
#endif // QT_GUI_LIB
            default:
                qFatal() << mm.returnMetaType() << "not supported yet";
            }

            break;
        }

        if (found)
            break;
    }

#ifdef QT_GUI_LIB
    if (!found) {
        for (const auto &pair : std::as_const(d->actions)) {
            const auto tool = pair.first;
            if (tool.name() != name)
                continue;
            auto action = pair.second;
            action->trigger();
            found = true;
            break;
        }
    }
#endif

    if (ok)
        *ok = found;
    if (!found)
        qWarning() << name << "not found for " << params;
    return ret;
}

QFuture<QList<QMcpCallToolResultContent>> QMcpServerSession::callToolAsync(
    const QString &name, const QJsonObject &params, const QVariant &progressToken)
{
    using namespace Qt::Literals::StringLiterals;

    for (const auto &pair : std::as_const(d->tools)) {
        const auto tool = pair.first;
        if (tool.name() != name)
            continue;

        const auto mo = pair.second->metaObject();
        for (int i = mo->methodOffset(); i < mo->methodCount(); ++i) {
            const auto mm = mo->method(i);
            if (mm.methodType() != QMetaMethod::Method)
                continue;
            if (mm.access() != QMetaMethod::Public)
                continue;
            if (QString::fromUtf8(mm.name()) != name)
                continue;

            // Check if return type is QFuture
            const QByteArray returnTypeName = mm.returnMetaType().name();
            if (!returnTypeName.startsWith("QFuture<"))
                continue;

            // Build converted arguments
            QVariantList convertedArgs;
            for (int j = 0; j < mm.parameterCount(); ++j) {
                const auto paramName = QString::fromUtf8(mm.parameterNames().at(j));
                const auto type = mm.parameterMetaType(j);

                // Handle special parameter types
                if (type.id() == QMetaType::QUuid) {
                    convertedArgs.append(d->sessionId);
                    continue;
                }

                if (!params.contains(paramName))
                    break;

                auto value = params.value(paramName).toVariant();
                if (!value.convert(type)) {
                    qWarning() << "Failed to convert parameter" << paramName;
                    break;
                }
                convertedArgs.append(value);
            }

            if (convertedArgs.count() != mm.parameterCount())
                continue;

            // Invoke the method and get the QFuture
            QFuture<QList<QMcpCallToolResultContent>> resultFuture;
            QGenericReturnArgument ret(mm.returnMetaType().name(), &resultFuture);

            switch (mm.parameterCount()) {
            case 0:
                mm.invoke(pair.second, ret);
                break;
            case 1:
                mm.invoke(pair.second, ret,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()));
                break;
            case 2:
                mm.invoke(pair.second, ret,
                          QGenericArgument(convertedArgs[0].typeName(), convertedArgs[0].constData()),
                          QGenericArgument(convertedArgs[1].typeName(), convertedArgs[1].constData()));
                break;
            default:
                qWarning() << "callToolAsync: too many parameters";
                break;
            }

            // Set up progress monitoring if progressToken is provided
            if (progressToken.isValid() && !progressToken.isNull()) {
                auto *watcher = new QFutureWatcher<QList<QMcpCallToolResultContent>>(this);
                auto *server = qobject_cast<QMcpServer *>(parent());

                connect(watcher, &QFutureWatcherBase::progressValueChanged, this,
                        [this, server, progressToken](int value) {
                    if (!server)
                        return;
                    QMcpProgressNotificationParams params;
                    params.setProgressToken(progressToken);
                    params.setProgress(value);
                    QMcpProgressNotification notification;
                    notification.setParams(params);
                    server->notify(d->sessionId, notification);
                });

                connect(watcher, &QFutureWatcherBase::progressRangeChanged, this,
                        [this, server, progressToken](int min, int max) {
                    Q_UNUSED(min);
                    if (!server)
                        return;
                    QMcpProgressNotificationParams params;
                    params.setProgressToken(progressToken);
                    params.setProgress(0);
                    params.setTotal(max);
                    QMcpProgressNotification notification;
                    notification.setParams(params);
                    server->notify(d->sessionId, notification);
                });

                connect(watcher, &QFutureWatcherBase::finished, watcher, &QObject::deleteLater);
                watcher->setFuture(resultFuture);
            }

            return resultFuture;
        }
    }

    // If no async tool found, try sync callTool and wrap result
    bool ok = false;
    auto syncResult = callTool(name, params, &ok);
    if (ok) {
        QPromise<QList<QMcpCallToolResultContent>> promise;
        promise.start();
        promise.addResult(syncResult);
        promise.finish();
        return promise.future();
    }

    // Return empty/canceled future if tool not found
    QPromise<QList<QMcpCallToolResultContent>> promise;
    promise.start();
    promise.finish();
    return promise.future();
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

void QMcpServerSession::createMessage(const QMcpCreateMessageRequestParams &params)
{
    auto server = qobject_cast<QMcpServer *>(parent());
    if (!server)
        return;
    QMcpCreateMessageRequest request;
    request.setParams(params);
    server->request(d->sessionId, request, [this, server](const QUuid &sessionId, const QMcpCreateMessageResult &result) {
        Q_ASSERT(d->sessionId == sessionId);
        emit createMessageFinished(result);
    });
}


QT_END_NAMESPACE
