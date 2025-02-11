#ifndef QMCPABSTRACTHTTPSERVER_H
#define QMCPABSTRACTHTTPSERVER_H

#include <QtCore/QObject>
#include <QtMcpServer/qmcpserverglobal.h>

QT_BEGIN_NAMESPACE

class QTcpServer;

class Q_MCPSERVER_EXPORT QMcpAbstractHttpServer : public QObject
{
    Q_OBJECT

public:
    explicit QMcpAbstractHttpServer(QObject *parent = nullptr);
    ~QMcpAbstractHttpServer() override;

    bool bind(QTcpServer *server);

private:
    class Private;
    QScopedPointer<Private> d;
};

QT_END_NAMESPACE

#endif // QMCPABSTRACTHTTPSERVER_H
