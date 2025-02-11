#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtMcpServer/qmcpabstracthttpserver.h>
#include <QtNetwork/QNetworkRequest>

class HttpServer : public QMcpAbstractHttpServer
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);

    Q_INVOKABLE QByteArray sse(const QNetworkRequest &request);
    Q_INVOKABLE QByteArray message();
};

#endif // HTTPSERVER_H
