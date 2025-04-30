// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/qmcpsamplingmessagecontent.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qmcpimagecontent.h>
#include <QtMcpCommon/qmcpaudiocontent.h>
#include <QtTest/QTest>

class tst_QMcpSamplingMessageContent : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void textContent();
    void imageContent();
    void audioContent();
    void copyConstructor();
    void assignmentOperator();
};

void tst_QMcpSamplingMessageContent::defaultValues()
{
    QMcpSamplingMessageContent content;
    QVERIFY(content.refType().isEmpty());
    QVERIFY(content.textContent().text().isEmpty());
    QVERIFY(content.imageContent().data().isEmpty());
    QVERIFY(content.audioContent().data().isEmpty());
}

void tst_QMcpSamplingMessageContent::textContent()
{
    // Create text content
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    
    // Set it in the sampling message content
    QMcpSamplingMessageContent content;
    content.setTextContent(textContent);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("textContent"));
    QCOMPARE(content.textContent().text(), QString("Hello, world!"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("text").toString(), QString("Hello, world!"));
    QCOMPARE(jsonObj.value("type").toString(), QString("text"));
    
    // Test JSON parsing
    QMcpSamplingMessageContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("textContent"));
    QCOMPARE(parsedContent.textContent().text(), QString("Hello, world!"));
}

void tst_QMcpSamplingMessageContent::imageContent()
{
    // Create image content
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    
    // Set it in the sampling message content
    QMcpSamplingMessageContent content;
    content.setImageContent(imageContent);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("imageContent"));
    QCOMPARE(content.imageContent().data(), QByteArray("image data"));
    QCOMPARE(content.imageContent().mimeType(), QString("image/png"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("data").toString(), QString("image data"));
    QCOMPARE(jsonObj.value("mimeType").toString(), QString("image/png"));
    QCOMPARE(jsonObj.value("type").toString(), QString("image"));
    
    // Test JSON parsing
    QMcpSamplingMessageContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("imageContent"));
    QCOMPARE(parsedContent.imageContent().data(), QByteArray("image data"));
    QCOMPARE(parsedContent.imageContent().mimeType(), QString("image/png"));
}

void tst_QMcpSamplingMessageContent::audioContent()
{
    // Create audio content
    QMcpAudioContent audioContent;
    audioContent.setData(QByteArray("audio data"));
    audioContent.setMimeType("audio/mp3");
    
    // Set it in the sampling message content
    QMcpSamplingMessageContent content;
    content.setAudioContent(audioContent);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("audioContent"));
    QCOMPARE(content.audioContent().data(), QByteArray("audio data"));
    QCOMPARE(content.audioContent().mimeType(), QString("audio/mp3"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("data").toString(), QString("audio data"));
    QCOMPARE(jsonObj.value("mimeType").toString(), QString("audio/mp3"));
    QCOMPARE(jsonObj.value("type").toString(), QString("audio"));
    
    // Test JSON parsing
    QMcpSamplingMessageContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("audioContent"));
    QCOMPARE(parsedContent.audioContent().data(), QByteArray("audio data"));
    QCOMPARE(parsedContent.audioContent().mimeType(), QString("audio/mp3"));
}

void tst_QMcpSamplingMessageContent::copyConstructor()
{
    // Create content with text
    QMcpSamplingMessageContent original;
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    original.setTextContent(textContent);
    
    // Test copy constructor
    QMcpSamplingMessageContent copy(original);
    QCOMPARE(copy.refType(), QByteArray("textContent"));
    QCOMPARE(copy.textContent().text(), QString("Hello, world!"));
    
    // Modify original, verify copy is unchanged
    QMcpTextContent newTextContent;
    newTextContent.setText("Modified text");
    original.setTextContent(newTextContent);
    QCOMPARE(copy.textContent().text(), QString("Hello, world!"));
    QCOMPARE(original.textContent().text(), QString("Modified text"));
}

void tst_QMcpSamplingMessageContent::assignmentOperator()
{
    // Create content with image
    QMcpSamplingMessageContent original;
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    original.setImageContent(imageContent);
    
    // Test assignment operator
    QMcpSamplingMessageContent assigned;
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

QTEST_MAIN(tst_QMcpSamplingMessageContent)
#include "tst_qmcpsamplingmessagecontent.moc"