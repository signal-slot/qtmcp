// Copyright (C) 2025 Signal Slot Inc.
// SPDX-License-Identifier: LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qmcpserverbackendinterface.h"

#include <QtMcpCommon/QMcpRequest>
#include <QtMcpCommon/QMcpResult>
#include <QtMcpCommon/QMcpJSONRPCRequest>
#include <QtMcpCommon/QMcpJSONRPCResponse>

QT_BEGIN_NAMESPACE

QMcpServerBackendInterface::QMcpServerBackendInterface(QObject *parent)
    : QObject{parent}
{
    connect(this, &QMcpServerBackendInterface::received, this, [this](const QJsonObject &object) {
        if (object.contains("id"_L1)) {
            const auto id = object.value("id"_L1);
            const auto result = object.value("result"_L1).toObject();
            if (callbacks.contains(id)) {
                callbacks.take(id)(result);
            }
            emit this->result(result);
        } else {
            // TODO: notification
        }
    });
}

void QMcpServerBackendInterface::request(const QJsonObject &request, std::function<void(const QJsonObject &)> callback)
{
    static int id = 0;
    if (request.contains("id"_L1)) {
        auto request2 = request;
        request2.insert("id"_L1, id);

        if (callback)
            callbacks.insert(id, callback);
        id++;
        send(request2);
    } else {
        send(request);
    }
}

QT_END_NAMESPACE
