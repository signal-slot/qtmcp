#include "httpserver.h"

HttpServer::HttpServer(QObject *parent)
    : QMcpAbstractHttpServer(parent)
{
}

QByteArray HttpServer::sse(const QNetworkRequest &)
{
    return QByteArray();
}

QByteArray HttpServer::message()
{
    return QByteArrayLiteral("Regular HTTP endpoint");
}
