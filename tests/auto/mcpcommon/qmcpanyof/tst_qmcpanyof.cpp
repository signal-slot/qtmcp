// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/QMcpAnyOf>
#include <QtTest/QTest>

// Test gadget classes for anyOf testing
class TestGadget1 : public QMcpGadget
{
    Q_GADGET
    Q_PROPERTY(QString type READ type WRITE setType FINAL)
    Q_PROPERTY(QString value READ value WRITE setValue FINAL)

public:
    TestGadget1() : QMcpGadget(new Private) {}

protected:
    TestGadget1(Private *d) : QMcpGadget(d) {}

public:
    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QString type() const { return d<Private>()->type; }
    void setType(const QString &type) {
        if (this->type() == type) return;
        d<Private>()->type = type;
    }

    QString value() const { return d<Private>()->value; }
    void setValue(const QString &value) {
        if (this->value() == value) return;
        d<Private>()->value = value;
    }

protected:
    struct Private : public QMcpGadget::Private {
        QString type;
        QString value;
        Private *clone() const override { return new Private(*this); }
    };
};

class TestGadget2 : public QMcpGadget
{
    Q_GADGET
    Q_PROPERTY(QString type READ type WRITE setType FINAL)
    Q_PROPERTY(int number READ number WRITE setNumber FINAL)

public:
    TestGadget2() : QMcpGadget(new Private) {}

protected:
    TestGadget2(Private *d) : QMcpGadget(d) {}

public:
    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    QString type() const { return d<Private>()->type; }
    void setType(const QString &type) {
        if (this->type() == type) return;
        d<Private>()->type = type;
    }

    int number() const { return d<Private>()->number; }
    void setNumber(int number) {
        if (this->number() == number) return;
        d<Private>()->number = number;
    }

protected:
    struct Private : public QMcpGadget::Private {
        QString type;
        int number = 0;
        Private *clone() const override { return new Private(*this); }
    };
};

class TestAnyOf : public QMcpAnyOf
{
    Q_GADGET
    Q_PROPERTY(TestGadget1 type1 READ type1 WRITE setType1 FINAL)
    Q_PROPERTY(TestGadget2 type2 READ type2 WRITE setType2 FINAL)

public:
    TestAnyOf() : QMcpAnyOf(new Private) {}

protected:
    TestAnyOf(Private *d) : QMcpAnyOf(d) {}

public:
    const QMetaObject* metaObject() const override {
        return &staticMetaObject;
    }

    // Make refType public for testing
    using QMcpAnyOf::refType;

    TestGadget1 type1() const { return d<Private>()->type1; }
    void setType1(const TestGadget1 &type1) {
        d<Private>()->type1 = type1;
    }

    TestGadget2 type2() const { return d<Private>()->type2; }
    void setType2(const TestGadget2 &type2) {
        d<Private>()->type2 = type2;
    }

protected:
    struct Private : public QMcpAnyOf::Private {
        TestGadget1 type1;
        TestGadget2 type2;
        Private *clone() const override { return new Private(*this); }
    };
};

class tst_QMcpAnyOf : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void convert_data();
    void convert();
    void copy_data();
    void copy();
};

void tst_QMcpAnyOf::defaultValues()
{
    TestAnyOf anyOf;
    QVERIFY(anyOf.refType().isEmpty());
}

void tst_QMcpAnyOf::convert_data()
{
    QTest::addColumn<QByteArray>("json");
    QTest::addColumn<QByteArray>("expectedType");
    QTest::addColumn<QVariantMap>("expectedData");

    // Test type1
    QTest::newRow("type1") << R"({
        "type": "type1",
        "value": "test value"
    })"_ba
    << QByteArray("type1")
    << QVariantMap {
        { "type", "type1" },
        { "value", "test value" }
    };

    // Test type2
    QTest::newRow("type2") << R"({
        "type": "type2",
        "number": 42
    })"_ba
    << QByteArray("type2")
    << QVariantMap {
        { "type", "type2" },
        { "number", 42 }
    };
}

void tst_QMcpAnyOf::convert()
{
    QFETCH(QByteArray, json);
    QFETCH(QByteArray, expectedType);
    QFETCH(QVariantMap, expectedData);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());
    const auto object = doc.object();

    TestAnyOf anyOf;
    QVERIFY(anyOf.fromJsonObject(object));
    QCOMPARE(anyOf.refType(), expectedType);
    TestHelper::verify(&anyOf, expectedData);

    const auto converted = anyOf.toJsonObject();
    const auto expectedObj = QJsonObject::fromVariantMap(expectedData);
    QCOMPARE(converted, expectedObj);
}

void tst_QMcpAnyOf::copy_data()
{
    convert_data();
}

void tst_QMcpAnyOf::copy()
{
    QFETCH(QByteArray, json);
    QFETCH(QByteArray, expectedType);
    QFETCH(QVariantMap, expectedData);

    TestAnyOf anyOf;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    QVERIFY(error.error == QJsonParseError::NoError);
    QVERIFY(doc.isObject());

    const auto object = doc.object();
    QVERIFY(anyOf.fromJsonObject(object));

    // Test copy constructor
    TestAnyOf anyOf2(anyOf);
    QCOMPARE(anyOf2.refType(), expectedType);
    QCOMPARE(anyOf2.toJsonObject(), QJsonObject::fromVariantMap(expectedData));

    // Test assignment operator
    TestAnyOf anyOf3;
    anyOf3 = anyOf2;
    QCOMPARE(anyOf3.refType(), expectedType);
    QCOMPARE(anyOf3.toJsonObject(), QJsonObject::fromVariantMap(expectedData));
}

QTEST_MAIN(tst_QMcpAnyOf)
#include "tst_qmcpanyof.moc"
