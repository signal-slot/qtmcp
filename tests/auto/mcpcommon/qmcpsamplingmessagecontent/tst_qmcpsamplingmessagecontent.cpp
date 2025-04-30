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
    QCOMPARE(content.refType(), "textContent"_ba);
    QCOMPARE(content.textContent().text(), "Hello, world!"_L1);

    // Test JSON conversion
    QJsonObject jsonObj = content.toJsonObject();
    QCOMPARE(jsonObj.value("text").toString(), "Hello, world!"_L1);
    QCOMPARE(jsonObj.value("type").toString(), "text"_L1);

    // Test JSON parsing
    QMcpSamplingMessageContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), "textContent"_ba);
    QCOMPARE(parsedContent.textContent().text(), "Hello, world!"_L1);
}

void tst_QMcpSamplingMessageContent::imageContent()
{
    // Create image content
    QMcpImageContent imageContent;
    imageContent.setData("image data"_ba);
    imageContent.setMimeType("image/png");

    // Set it in the sampling message content
    QMcpSamplingMessageContent content;
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
    QMcpSamplingMessageContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), "imageContent"_ba);
    QCOMPARE(parsedContent.imageContent().data(), "image data"_ba);
    QCOMPARE(parsedContent.imageContent().mimeType(), "image/png"_L1);
}

void tst_QMcpSamplingMessageContent::audioContent()
{
    // Create audio content
    QMcpAudioContent audioContent;
    audioContent.setData("audio data"_ba);
    audioContent.setMimeType("audio/mp3");

    // Set it in the sampling message content
    QMcpSamplingMessageContent content;
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
    QMcpSamplingMessageContent parsedContent;
    QVERIFY(parsedContent.fromJsonObject(jsonObj));
    QCOMPARE(parsedContent.refType(), "audioContent"_ba);
    QCOMPARE(parsedContent.audioContent().data(), "audio data"_ba);
    QCOMPARE(parsedContent.audioContent().mimeType(), "audio/mp3"_L1);
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
    QCOMPARE(copy.refType(), "textContent"_ba);
    QCOMPARE(copy.textContent().text(), "Hello, world!"_L1);

    // Modify original, verify copy is unchanged
    QMcpTextContent newTextContent;
    newTextContent.setText("Modified text");
    original.setTextContent(newTextContent);
    QCOMPARE(copy.textContent().text(), "Hello, world!"_L1);
    QCOMPARE(original.textContent().text(), "Modified text"_L1);
}

void tst_QMcpSamplingMessageContent::assignmentOperator()
{
    // Create content with image
    QMcpSamplingMessageContent original;
    QMcpImageContent imageContent;
    imageContent.setData("image data"_ba);
    imageContent.setMimeType("image/png");
    original.setImageContent(imageContent);

    // Test assignment operator
    QMcpSamplingMessageContent assigned;
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

QTEST_MAIN(tst_QMcpSamplingMessageContent)
#include "tst_qmcpsamplingmessagecontent.moc"
