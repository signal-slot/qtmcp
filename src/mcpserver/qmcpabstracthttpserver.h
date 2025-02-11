#ifndef QMCPABSTRACTHTTPSERVER_H
#define QMCPABSTRACTHTTPSERVER_H

#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtMcpServer/qmcpserverglobal.h>
#include <QtNetwork/QNetworkRequest>

QT_BEGIN_NAMESPACE

class QTcpServer;

class Q_MCPSERVER_EXPORT QMcpAbstractHttpServer : public QObject
{
    Q_OBJECT

public:
    explicit QMcpAbstractHttpServer(QObject *parent = nullptr);
    ~QMcpAbstractHttpServer() override;

    bool bind(QTcpServer *server);

protected:
    QByteArray registerSseRequest(const QNetworkRequest &request);
    void sendSseEvent(const QByteArray &id, const QByteArray &data, const QString &event = QString());
    void closeSseConnection(const QByteArray &id);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPABSTRACTHTTPSERVER_H
