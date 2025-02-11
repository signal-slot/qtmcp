#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtMcpServer/qmcpabstracthttpserver.h>
#include <QtNetwork/QNetworkRequest>
#include <QtCore/QSet>
#include <QtCore/QHash>

class HttpServer : public QMcpAbstractHttpServer
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer() override;

    Q_INVOKABLE QByteArray getSse(const QNetworkRequest &request);
    Q_INVOKABLE QByteArray postMessages(const QNetworkRequest &request, const QByteArray &body);

public slots:
    void send(const QUuid &session, const QJsonObject &object);

signals:
    void newSession(const QUuid &session);
    void received(const QUuid &session, const QJsonObject &object);

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // HTTPSERVER_H
