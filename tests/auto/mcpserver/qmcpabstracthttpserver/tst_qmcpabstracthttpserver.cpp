#include <QTest>
#include <QtCore/QUrlQuery>
#include <QtCore/QTimer>
#include <QtMcpServer/qmcpabstracthttpserver.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QTcpServer>

class TestHttpServer : public QMcpAbstractHttpServer
{
    Q_OBJECT
public:
    explicit TestHttpServer(QObject *parent = nullptr) : QMcpAbstractHttpServer(parent) {}
protected:
    Q_INVOKABLE QByteArray get() const;
    Q_INVOKABLE QByteArray getEcho(const QNetworkRequest &request) const;
    Q_INVOKABLE QByteArray post(const QNetworkRequest &request, const QByteArray &body) const;
    Q_INVOKABLE QByteArray postEcho(const QNetworkRequest &request, const QByteArray &body) const;
};

QByteArray TestHttpServer::get() const
{
    return "/";
}

QByteArray TestHttpServer::getEcho(const QNetworkRequest &request) const
{
    QUrlQuery query(request.url().query());
    return query.queryItemValue("message").toUtf8();
}

QByteArray TestHttpServer::post(const QNetworkRequest &request, const QByteArray &body) const
{
    return "";
}

QByteArray TestHttpServer::postEcho(const QNetworkRequest &request, const QByteArray &body) const
{
    return body;
}

class tst_QMcpAbstractHttpServer : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void testBind();
    void testGet_data();
    void testGet();
    void testPost_data();
    void testPost();

private:
    QNetworkAccessManager nam;
    TestHttpServer *server = nullptr;
    QTcpServer *tcpServer = nullptr;
    const int port = 45678;
};

void tst_QMcpAbstractHttpServer::init()
{
    server = new TestHttpServer(this);
    tcpServer = new QTcpServer(this);
    QVERIFY(tcpServer->listen(QHostAddress::LocalHost, port));
    QCOMPARE(tcpServer->serverPort(), port);
    QVERIFY(server->bind(tcpServer));
}

void tst_QMcpAbstractHttpServer::cleanup()
{
    delete server;
    delete tcpServer;
    server = nullptr;
    tcpServer = nullptr;
}

void tst_QMcpAbstractHttpServer::testBind()
{
    QTcpServer *newServer = new QTcpServer(this);
    QVERIFY(newServer->listen(QHostAddress::LocalHost, port + 1));
    QVERIFY(server->bind(newServer));
    delete newServer;
}

void tst_QMcpAbstractHttpServer::testGet_data()
{
    QTest::addColumn<QLatin1String>("path");
    QTest::addColumn<QLatin1String>("query");
    QTest::addColumn<QByteArray>("expectedResponse");

    QTest::newRow("root") << "/"_L1 << ""_L1 << "/"_ba;
    QTest::newRow("echo") << "/echo"_L1 << "message=abc"_L1 << "abc"_ba;
}

void tst_QMcpAbstractHttpServer::testGet()
{
    QFETCH(QLatin1String, path);
    QFETCH(QLatin1String, query);
    QFETCH(QByteArray, expectedResponse);

    QUrl url("http://127.0.0.1/");
    url.setPort(port);
    url.setPath(path);
    if (!query.isEmpty())
        url.setQuery(query);
    QNetworkRequest request(url);

    QNetworkReply *reply = nam.get(request);
    connect(reply, &QNetworkReply::errorOccurred, [reply](QNetworkReply::NetworkError error) {
        qWarning() << "Error:" << error << reply->errorString();
    });
    QEventLoop loop;
    QTimer::singleShot(1000, this, [&loop]() {
        loop.quit();
    });
    connect(reply, &QNetworkReply::finished, this, [reply, &loop]() {
        reply->deleteLater();
        loop.quit();
    });
    loop.exec();
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QCOMPARE(reply->readAll(), expectedResponse);
}

void tst_QMcpAbstractHttpServer::testPost_data()
{
    QTest::addColumn<QLatin1String>("path");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("expectedResponse");

    QTest::newRow("root") << "/"_L1 << ""_ba << ""_ba;
    QTest::newRow("echo") << "/echo"_L1 << "abc"_ba << "abc"_ba;
}

void tst_QMcpAbstractHttpServer::testPost()
{
    QFETCH(QLatin1String, path);
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, expectedResponse);

    QUrl url("http://127.0.0.1/");
    url.setPort(port);
    url.setPath(path);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded"_ba);

    QNetworkReply *reply = nam.post(request, data);
    connect(reply, &QNetworkReply::errorOccurred, [reply](QNetworkReply::NetworkError error) {
        qWarning() << "Error:" << error << reply->errorString();
    });
    QEventLoop loop;
    QTimer::singleShot(1000, this, [&loop]() {
        loop.quit();
    });
    connect(reply, &QNetworkReply::finished, this, [reply, &loop]() {
        reply->deleteLater();
        loop.quit();
    });
    loop.exec();
    QCOMPARE(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt(), 200);
    QCOMPARE(reply->readAll(), expectedResponse);
}

QTEST_MAIN(tst_QMcpAbstractHttpServer)
#include "tst_qmcpabstracthttpserver.moc"
