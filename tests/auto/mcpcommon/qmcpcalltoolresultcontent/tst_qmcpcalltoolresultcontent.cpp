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
    QCOMPARE(content.refType(), "textContent"_ba);
    QCOMPARE(content.textContent().text(), "Hello, world!"_L1);

    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("text").toString(), "Hello, world!"_L1);
    QCOMPARE(jsonObj.value("type").toString(), "text"_L1);

    // Test JSON parsing
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), "textContent"_ba);
    QCOMPARE(parsedContent.textContent().text(), "Hello, world!"_L1);
}

void tst_QMcpCallToolResultContent::imageContent()
{
    // Create image content
    QMcpImageContent imageContent;
    imageContent.setData("image data"_ba);
    imageContent.setMimeType("image/png");

    // Set it in the call tool result content
    QMcpCallToolResultContent content;
    content.setImageContent(imageContent);

    // Verify the content
    QCOMPARE(content.refType(), "imageContent"_ba);
    QCOMPARE(content.imageContent().data(), "image data"_ba);
    QCOMPARE(content.imageContent().mimeType(), "image/png"_L1);

    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("data").toString(), "image data"_L1);
    QCOMPARE(jsonObj.value("mimeType").toString(), "image/png"_L1);
    QCOMPARE(jsonObj.value("type").toString(), "image"_L1);

    // Test JSON parsing
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), "imageContent"_ba);
    QCOMPARE(parsedContent.imageContent().data(), "image data"_ba);
    QCOMPARE(parsedContent.imageContent().mimeType(), "image/png"_L1);
}

void tst_QMcpCallToolResultContent::audioContent()
{
    // Create audio content
    QMcpAudioContent audioContent;
    audioContent.setData("audio data"_ba);
    audioContent.setMimeType("audio/mp3");

    // Set it in the call tool result content
    QMcpCallToolResultContent content;
    content.setAudioContent(audioContent);

    // Verify the content
    QCOMPARE(content.refType(), "audioContent"_ba);
    QCOMPARE(content.audioContent().data(), "audio data"_ba);
    QCOMPARE(content.audioContent().mimeType(), "audio/mp3"_L1);

    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("data").toString(), "audio data"_L1);
    QCOMPARE(jsonObj.value("mimeType").toString(), "audio/mp3"_L1);
    QCOMPARE(jsonObj.value("type").toString(), "audio"_L1);

    // Test JSON parsing
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), "audioContent"_ba);
    QCOMPARE(parsedContent.audioContent().data(), "audio data"_ba);
    QCOMPARE(parsedContent.audioContent().mimeType(), "audio/mp3"_L1);
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
    QCOMPARE(content.refType(), "embeddedResource"_ba);
    QCOMPARE(content.embeddedResource().resource().refType(), "textResourceContents"_ba);
    QCOMPARE(content.embeddedResource().resource().textResourceContents().uri(), QUrl("resource://test"));

    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("resource").toObject().value("uri").toString(), "resource://test"_L1);

    // Test JSON parsing
    QMcpCallToolResultContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), "embeddedResource"_ba);
    QCOMPARE(parsedContent.embeddedResource().resource().refType(), "textResourceContents"_ba);
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
    QCOMPARE(copy.refType(), "textContent"_ba);
    QCOMPARE(copy.textContent().text(), "Hello, world!"_L1);

    // Modify original, verify copy is unchanged
    QMcpTextContent newTextContent;
    newTextContent.setText("Modified text");
    original.setTextContent(newTextContent);
    QCOMPARE(copy.textContent().text(), "Hello, world!"_L1);
    QCOMPARE(original.textContent().text(), "Modified text"_L1);
}

void tst_QMcpCallToolResultContent::assignmentOperator()
{
    // Create content with image
    QMcpCallToolResultContent original;
    QMcpImageContent imageContent;
    imageContent.setData("image data"_ba);
    imageContent.setMimeType("image/png");
    original.setImageContent(imageContent);

    // Test assignment operator
    QMcpCallToolResultContent assigned;
    assigned = original;
    QCOMPARE(assigned.refType(), "imageContent"_ba);
    QCOMPARE(assigned.imageContent().data(), "image data"_ba);
    QCOMPARE(assigned.imageContent().mimeType(), "image/png"_L1);

    // Modify original, verify assigned is unchanged
    QMcpImageContent newImageContent;
    newImageContent.setData("new image data"_ba);
    newImageContent.setMimeType("image/jpeg");
    original.setImageContent(newImageContent);
    QCOMPARE(assigned.imageContent().data(), "image data"_ba);
    QCOMPARE(assigned.imageContent().mimeType(), "image/png"_L1);
    QCOMPARE(original.imageContent().data(), "new image data"_ba);
    QCOMPARE(original.imageContent().mimeType(), "image/jpeg"_L1);
}

void tst_QMcpCallToolResultContent::constructors()
{
    // Test constructor with text content
    QMcpTextContent textContent;
    textContent.setText("Hello, world!");
    QMcpCallToolResultContent textResult(textContent);
    QCOMPARE(textResult.refType(), "textContent"_ba);
    QCOMPARE(textResult.textContent().text(), "Hello, world!"_L1);

    // Test constructor with image content
    QMcpImageContent imageContent;
    imageContent.setData("image data"_ba);
    imageContent.setMimeType("image/png");
    QMcpCallToolResultContent imageResult(imageContent);
    QCOMPARE(imageResult.refType(), "imageContent"_ba);
    QCOMPARE(imageResult.imageContent().data(), "image data"_ba);
    QCOMPARE(imageResult.imageContent().mimeType(), "image/png"_L1);

    // Test constructor with audio content
    QMcpAudioContent audioContent;
    audioContent.setData("audio data"_ba);
    audioContent.setMimeType("audio/mp3");
    QMcpCallToolResultContent audioResult(audioContent);
    QCOMPARE(audioResult.refType(), "audioContent"_ba);
    QCOMPARE(audioResult.audioContent().data(), "audio data"_ba);
    QCOMPARE(audioResult.audioContent().mimeType(), "audio/mp3"_L1);

    // Test constructor with embedded resource
    QMcpEmbeddedResource embeddedResource;
    QMcpTextResourceContents textResourceContents;
    textResourceContents.setUri(QUrl("resource://test"));
    textResourceContents.setText("Test resource content");

    QMcpEmbeddedResourceResource resource;
    resource.setTextResourceContents(textResourceContents);
    embeddedResource.setResource(resource);
    QMcpCallToolResultContent resourceResult(embeddedResource);
    QCOMPARE(resourceResult.refType(), "embeddedResource"_ba);
    QCOMPARE(resourceResult.embeddedResource().resource().refType(), "textResourceContents"_ba);
    QCOMPARE(resourceResult.embeddedResource().resource().textResourceContents().uri(), QUrl("resource://test"));
}

QTEST_MAIN(tst_QMcpCallToolResultContent)
#include "tst_qmcpcalltoolresultcontent.moc"
