#include "httpserver.h"

HttpServer::HttpServer(QObject *parent)
    : QMcpAbstractHttpServer{parent}
{}

QByteArray HttpServer::sse()
{
    return QByteArrayLiteral("");
}

QByteArray HttpServer::message()
{
    return QByteArrayLiteral("");
}
