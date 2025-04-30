// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "../testhelper.h"
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonParseError>
#include <QtMcpCommon/qmcpcalltoolresultcontent.h>
#include <QtMcpCommon/qmcptextcontent.h>
#include <QtMcpCommon/qmcpimagecontent.h>
#include <QtMcpCommon/qmcpaudiocontent.h>
#include <QtMcpCommon/qmcpembeddedresource.h>
#include <QtTest/QTest>

class tst_QMcpCallToolResultContent : public QObject
{
    Q_OBJECT

private slots:
    void defaultValues();
    void textContent();
    void imageContent();
    void audioContent();
    void embeddedResource();
    void copyConstructor();
    void assignmentOperator();
    void constructors();
};

void tst_QMcpCallToolResultContent::defaultValues()
{
    QMcpCallToolResultContent content;
    QVERIFY(content.refType().isEmpty());
    QVERIFY(content.textContent().text().isEmpty());
    QVERIFY(content.imageContent().data().isEmpty());
    QVERIFY(content.audioContent().data().isEmpty());
    QVERIFY(content.embeddedResource().resource().refType().isEmpty());
}

void tst_QMcpCallToolResultContent::textContent()
{
    // Create text content
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    
    // Set it in the call tool result content
    QMcpCallToolResultContent content;
    content.setTextContent(textContent);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("textContent"));
    QCOMPARE(content.textContent().text(), QString("Hello, world!"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("text").toString(), QString("Hello, world!"));
    QCOMPARE(jsonObj.value("type").toString(), QString("text"));
    
    // Test JSON parsing
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("textContent"));
    QCOMPARE(parsedContent.textContent().text(), QString("Hello, world!"));
}

void tst_QMcpCallToolResultContent::imageContent()
{
    // Create image content
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    
    // Set it in the call tool result content
    QMcpCallToolResultContent content;
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
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("imageContent"));
    QCOMPARE(parsedContent.imageContent().data(), QByteArray("image data"));
    QCOMPARE(parsedContent.imageContent().mimeType(), QString("image/png"));
}

void tst_QMcpCallToolResultContent::audioContent()
{
    // Create audio content
    QMcpAudioContent audioContent;
    audioContent.setData(QByteArray("audio data"));
    audioContent.setMimeType("audio/mp3");
    
    // Set it in the call tool result content
    QMcpCallToolResultContent content;
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
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("audioContent"));
    QCOMPARE(parsedContent.audioContent().data(), QByteArray("audio data"));
    QCOMPARE(parsedContent.audioContent().mimeType(), QString("audio/mp3"));
}

void tst_QMcpCallToolResultContent::embeddedResource()
{
    QMcpTextResourceContents textResourceContents;
    textResourceContents.setUri(QUrl("resource://test"));
    textResourceContents.setText("Test resource content");
    
    QMcpEmbeddedResourceResource resource;
    resource.setTextResourceContents(textResourceContents);

    // Create embedded resource
    QMcpEmbeddedResource embeddedResource;
    embeddedResource.setResource(resource);
    
    // Set it in the call tool result content
    QMcpCallToolResultContent content;
    content.setEmbeddedResource(embeddedResource);
    
    // Verify the content
    QCOMPARE(content.refType(), QByteArray("embeddedResource"));
    QCOMPARE(content.embeddedResource().resource().refType(), QByteArray("textResourceContents"));
    QCOMPARE(content.embeddedResource().resource().textResourceContents().uri(), QUrl("resource://test"));
    
    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("resource").toObject().value("uri").toString(), QString("resource://test"));
    
    // Test JSON parsing
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), QByteArray("embeddedResource"));
    QCOMPARE(parsedContent.embeddedResource().resource().refType(), QByteArray("textResourceContents"));
    QCOMPARE(parsedContent.embeddedResource().resource().textResourceContents().uri(), QUrl("resource://test"));
}

void tst_QMcpCallToolResultContent::copyConstructor()
{
    // Create content with text
    QMcpCallToolResultContent original;
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    original.setTextContent(textContent);
    
    // Test copy constructor
    QMcpCallToolResultContent copy(original);
    QCOMPARE(copy.refType(), QByteArray("textContent"));
    QCOMPARE(copy.textContent().text(), QString("Hello, world!"));
    
    // Modify original, verify copy is unchanged
    QMcpTextContent newTextContent;
    newTextContent.setText("Modified text");
    original.setTextContent(newTextContent);
    QCOMPARE(copy.textContent().text(), QString("Hello, world!"));
    QCOMPARE(original.textContent().text(), QString("Modified text"));
}

void tst_QMcpCallToolResultContent::assignmentOperator()
{
    // Create content with image
    QMcpCallToolResultContent original;
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    original.setImageContent(imageContent);
    
    // Test assignment operator
    QMcpCallToolResultContent assigned;
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

void tst_QMcpCallToolResultContent::constructors()
{
    // Test constructor with text content
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    QMcpCallToolResultContent textResult(textContent);
    QCOMPARE(textResult.refType(), QByteArray("textContent"));
    QCOMPARE(textResult.textContent().text(), QString("Hello, world!"));
    
    // Test constructor with image content
    QMcpImageContent imageContent;
    imageContent.setData(QByteArray("image data"));
    imageContent.setMimeType("image/png");
    QMcpCallToolResultContent imageResult(imageContent);
    QCOMPARE(imageResult.refType(), QByteArray("imageContent"));
    QCOMPARE(imageResult.imageContent().data(), QByteArray("image data"));
    QCOMPARE(imageResult.imageContent().mimeType(), QString("image/png"));
    
    // Test constructor with audio content
    QMcpAudioContent audioContent;
    audioContent.setData(QByteArray("audio data"));
    audioContent.setMimeType("audio/mp3");
    QMcpCallToolResultContent audioResult(audioContent);
    QCOMPARE(audioResult.refType(), QByteArray("audioContent"));
    QCOMPARE(audioResult.audioContent().data(), QByteArray("audio data"));
    QCOMPARE(audioResult.audioContent().mimeType(), QString("audio/mp3"));
    
    // Test constructor with embedded resource
    QMcpEmbeddedResource embeddedResource;
    QMcpTextResourceContents textResourceContents;
    textResourceContents.setUri(QUrl("resource://test"));
    textResourceContents.setText("Test resource content");
    
    QMcpEmbeddedResourceResource resource;
    resource.setTextResourceContents(textResourceContents);
    embeddedResource.setResource(resource);
    QMcpCallToolResultContent resourceResult(embeddedResource);
    QCOMPARE(resourceResult.refType(), QByteArray("embeddedResource"));
    QCOMPARE(resourceResult.embeddedResource().resource().refType(), QByteArray("textResourceContents"));
    QCOMPARE(resourceResult.embeddedResource().resource().textResourceContents().uri(), QUrl("resource://test"));
}

QTEST_MAIN(tst_QMcpCallToolResultContent)
#include "tst_qmcpcalltoolresultcontent.moc"
