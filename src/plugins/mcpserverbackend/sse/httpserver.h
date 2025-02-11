#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QtMcpServer/qmcpabstracthttpserver.h>

class HttpServer : public QMcpAbstractHttpServer
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);

    Q_INVOKABLE QByteArray sse();
    Q_INVOKABLE QByteArray message();
};

#endif // HTTPSERVER_H
