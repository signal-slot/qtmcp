// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMCPSERVERSESSION_H
#define QMCPSERVERSESSION_H

#include <QtCore/QHash>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPair>
#include <QtCore/QScopedPointer>
#include <QtCore/QUrl>
#include <QtCore/QUuid>
#include <QtMcpCommon/QMcpCallToolResultContent>
#include <QtMcpCommon/QMcpPrompt>
#include <QtMcpCommon/QMcpPromptMessage>
#include <QtMcpCommon/QMcpReadResourceResultContents>
#include <QtMcpCommon/QMcpResource>
#include <QtMcpCommon/QMcpRoot>
#include <QtMcpServer/qmcpserverglobal.h>

QT_BEGIN_NAMESPACE

class QMcpServer;

class Q_MCPSERVER_EXPORT QMcpServerSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool initialized READ isInitialized WRITE setInitialized NOTIFY initializedChanged FINAL)
public:
    explicit QMcpServerSession(const QUuid &sessionId, QMcpServer *parent = nullptr);
    ~QMcpServerSession() override;

    QUuid sessionId() const;
    bool isInitialized() const;
    void setInitialized(bool initialized);

    // Resource management
    bool isSubscribed(const QUrl &uri) const;
    QList<QMcpResource> resources() const;
    QList<QMcpReadResourceResultContents> contents(const QUrl &uri) const;

    // Prompt management
    void appendPrompt(const QMcpPrompt &prompt, const QMcpPromptMessage &message);
    void insertPrompt(int index, const QMcpPrompt &prompt, const QMcpPromptMessage &message);
    void replacePrompt(int index, const QMcpPrompt prompt, const QMcpPromptMessage &message);
    void removePromptAt(int index);
    QList<QMcpPrompt> prompts(QString *cursor) const;
    QList<QMcpPromptMessage> messages(const QString &name) const;

    QList<QMcpCallToolResultContent> callTool(const QString &name, const QJsonObject &params, bool *ok = nullptr);

    QList<QMcpRoot> roots() const;

public slots:
    void appendResource(const QMcpResource &resource, const QMcpReadResourceResultContents &content);
    void insertResource(int index, const QMcpResource &resource, const QMcpReadResourceResultContents &content);
    void replaceResource(int index, const QMcpResource resource, const QMcpReadResourceResultContents &content);
    void removeResourceAt(int index);

    void subscribe(const QUrl &uri);
    void unsubscribe(const QUrl &uri);

    void setRoots(const QList<QMcpRoot> &roots);

signals:
    void initializedChanged(bool initialized);
    void resourceUpdated(const QMcpResource &resource);
    void rootsChanged(const QList<QMcpRoot> &roots);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPSERVERSESSION_H
