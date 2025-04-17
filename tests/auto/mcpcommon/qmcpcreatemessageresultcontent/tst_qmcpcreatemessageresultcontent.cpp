// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/qmcpcreatemessageresultcontent.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qmcpimagecontent.h>
#include <QtMcpCommon/qmcpaudiocontent.h>
#include <QtTest/QTest>

class tst_QMcpCreateMessageResultContent : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void textContent();
    void imageContent();
    void audioContent();
    void copyConstructor();
    void assignmentOperator();
    void constructors();
};

void tst_QMcpCreateMessageResultContent::defaultValues()
{
    QMcpCreateMessageResultContent content;
    QVERIFY(content.refType().isEmpty());
    QVERIFY(content.textContent().text().isEmpty());
    QVERIFY(content.imageContent().data().isEmpty());
    QVERIFY(content.audioContent().data().isEmpty());
}

void tst_QMcpCreateMessageResultContent::textContent()
{
    // Create text content
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    
    // Set it in the create message result content
    QMcpCreateMessageResultContent content;
    content.setTextContent(textContent);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("textContent"));
    QCOMPARE(content.textContent().text(), QString("Hello, world!"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj["text"].toString(), QString("Hello, world!"));
    QCOMPARE(jsonObj["type"].toString(), QString("text"));
    
    // Test JSON parsing
    QMcpCreateMessageResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("textContent"));
    QCOMPARE(parsedContent.textContent().text(), QString("Hello, world!"));
}

void tst_QMcpCreateMessageResultContent::imageContent()
{
    // Create image content
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    
    // Set it in the create message result content
    QMcpCreateMessageResultContent content;
    content.setImageContent(imageContent);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("imageContent"));
    QCOMPARE(content.imageContent().data(), QByteArray("image data"));
    QCOMPARE(content.imageContent().mimeType(), QString("image/png"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj["data"].toString(), QString("image data"));
    QCOMPARE(jsonObj["mimeType"].toString(), QString("image/png"));
    QCOMPARE(jsonObj["type"].toString(), QString("image"));
    
    // Test JSON parsing
    QMcpCreateMessageResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("imageContent"));
    QCOMPARE(parsedContent.imageContent().data(), QByteArray("image data"));
    QCOMPARE(parsedContent.imageContent().mimeType(), QString("image/png"));
}

void tst_QMcpCreateMessageResultContent::audioContent()
{
    // Create audio content
    QMcpAudioContent audioContent;
    audioContent.setData(QByteArray("audio data"));
    audioContent.setMimeType("audio/mp3");
    
    // Set it in the create message result content
    QMcpCreateMessageResultContent content;
    content.setAudioContent(audioContent);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("audioContent"));
    QCOMPARE(content.audioContent().data(), QByteArray("audio data"));
    QCOMPARE(content.audioContent().mimeType(), QString("audio/mp3"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj["data"].toString(), QString("audio data"));
    QCOMPARE(jsonObj["mimeType"].toString(), QString("audio/mp3"));
    QCOMPARE(jsonObj["type"].toString(), QString("audio"));
    
    // Test JSON parsing
    QMcpCreateMessageResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("audioContent"));
    QCOMPARE(parsedContent.audioContent().data(), QByteArray("audio data"));
    QCOMPARE(parsedContent.audioContent().mimeType(), QString("audio/mp3"));
}

void tst_QMcpCreateMessageResultContent::copyConstructor()
{
    // Create content with text
    QMcpCreateMessageResultContent original;
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    original.setTextContent(textContent);
    
    // Test copy constructor
    QMcpCreateMessageResultContent copy(original);
    QCOMPARE(copy.refType(), QByteArray("textContent"));
    QCOMPARE(copy.textContent().text(), QString("Hello, world!"));
    
    // Modify original, verify copy is unchanged
    QMcpTextContent newTextContent;
    newTextContent.setText("Modified text");
    original.setTextContent(newTextContent);
    QCOMPARE(copy.textContent().text(), QString("Hello, world!"));
    QCOMPARE(original.textContent().text(), QString("Modified text"));
}

void tst_QMcpCreateMessageResultContent::assignmentOperator()
{
    // Create content with image
    QMcpCreateMessageResultContent original;
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    original.setImageContent(imageContent);
    
    // Test assignment operator
    QMcpCreateMessageResultContent assigned;
    assigned = original;
    QCOMPARE(assigned.refType(), QByteArray("imageContent"));
    QCOMPARE(assigned.imageContent().data(), QByteArray("image data"));
    QCOMPARE(assigned.imageContent().mimeType(), QString("image/png"));
    
    // Modify original, verify assigned is unchanged
    QMcpImageContent newImageContent;
    newImageContent.setData(QByteArray("new image data"));
    newImageContent.setMimeType("image/jpeg");
    original.setImageContent(newImageContent);
    QCOMPARE(assigned.imageContent().data(), QByteArray("image data"));
    QCOMPARE(assigned.imageContent().mimeType(), QString("image/png"));
    QCOMPARE(original.imageContent().data(), QByteArray("new image data"));
    QCOMPARE(original.imageContent().mimeType(), QString("image/jpeg"));
}

void tst_QMcpCreateMessageResultContent::constructors()
{
    // Test constructor with text content
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    QMcpCreateMessageResultContent textResult(textContent);
    QCOMPARE(textResult.refType(), QByteArray("textContent"));
    QCOMPARE(textResult.textContent().text(), QString("Hello, world!"));
    
    // Test constructor with image content
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    QMcpCreateMessageResultContent imageResult(imageContent);
    QCOMPARE(imageResult.refType(), QByteArray("imageContent"));
    QCOMPARE(imageResult.imageContent().data(), QByteArray("image data"));
    QCOMPARE(imageResult.imageContent().mimeType(), QString("image/png"));
    
    // Test constructor with audio content
    QMcpAudioContent audioContent;
    audioContent.setData(QByteArray("audio data"));
    audioContent.setMimeType("audio/mp3");
    QMcpCreateMessageResultContent audioResult(audioContent);
    QCOMPARE(audioResult.refType(), QByteArray("audioContent"));
    QCOMPARE(audioResult.audioContent().data(), QByteArray("audio data"));
    QCOMPARE(audioResult.audioContent().mimeType(), QString("audio/mp3"));
}

QTEST_MAIN(tst_QMcpCreateMessageResultContent)
#include "tst_qmcpcreatemessageresultcontent.moc"