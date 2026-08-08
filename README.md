# Qt MCP (Model Context Protocol)

Qt MCP is a Qt-based implementation of the [Model Context Protocol](https://modelcontextprotocol.io/),
providing client and server libraries for applications that talk to language
models and AI tooling.

Qt MCP is developed by [Signal Slot Inc.](https://signal-slot.co.jp/) and is
**not part of the official Qt framework**. The recommended way to consume it
is through its own CMake package and namespace:

```cmake
find_package(QtMcp REQUIRED)
target_link_libraries(app PRIVATE QtMcp::Server)   # or QtMcp::Client / QtMcp::Common
```

The library is built with Qt's module build system, so the same modules are
also reachable as `Qt::McpCommon`/`Qt::McpClient`/`Qt::McpServer` via
`find_package(Qt6 COMPONENTS McpServer)`. That spelling keeps working, but
new code should prefer the `QtMcp::` namespace — the library behaves like a
Qt add-on module, but it is maintained independently.

## Table of Contents
- [Supported Protocol Revisions](#supported-protocol-revisions)
- [Transports](#transports)
- [Requirements](#requirements)
- [Building from Source](#building-from-source)
- [Getting Started](#getting-started)
- [Features](#features)
- [Project Structure](#project-structure)
- [Testing](#testing)
- [License](#license)
- [Contributing](#contributing)

## Supported Protocol Revisions

Qt MCP implements **every published MCP protocol revision**. A server
negotiates the revision per session, so a single server can talk to clients
of different revisions at the same time; messages are serialized in the wire
format of the negotiated revision.

| Revision | Highlights | Status |
|---|---|---|
| 2024-11-05 | Initial protocol | ✅ |
| 2025-03-26 | Audio content, annotations, model hints | ✅ |
| 2025-06-18 | Elicitation, structured tool output, resource links, `title`/`_meta` | ✅ |
| 2025-11-25 | Icons, URL mode elicitation, select-style enum schemas, tool calls in sampling | ✅ |
| 2026-07-28 | Stateless lifecycle, `server/discover`, MRTR, `subscriptions/listen`, required `resultType` | ✅ |

The official schemas of all revisions are kept in `spec/`.

Also implemented:

- **Multi Round-Trip Requests (MRTR, 2026-07-28)** — a handler that needs
  more input calls `QMcpServerSession::requireInput()` and answers with an
  `input_required` interim result; the client retries with `inputResponses`.
  On the client an interim result raises `QMcpClient::inputRequired`.
- **Tasks extension (`io.modelcontextprotocol/tasks`)** — enable with
  `QMcpServer::setTasksExtensionEnabled()` /
  `QMcpClient::setTasksExtensionEnabled()`; long-running tool calls then
  return a durable task handle polled via `tasks/get`.

## Transports

Both the client and the server load their transport as a plugin, selected by
name in the constructor:

| Plugin key | Transport | Notes |
|---|---|---|
| `stdio` | Standard input/output | Server runs as a subprocess of the client |
| `streamablehttp` | Streamable HTTP | Current HTTP transport; supports both the sessionful (2025-03-26 – 2025-11-25, `Mcp-Session-Id`) and sessionless (2026-07-28, header validation, `subscriptions/listen` streams) generations |
| `sse` | HTTP+SSE | Legacy 2024-11-05 transport, deprecated by the spec since 2025-03-26; kept for compatibility |

## Requirements

- Qt 6.8 or later
- CMake 3.16.0 or later
- C++20 compatible compiler

### Required Qt Components
- Qt Core
- Qt BuildInternals
- Qt Network (for the HTTP transports)

### Optional Qt Components
- Qt Gui (for QImage tool results)
- Qt Widgets (for GUI examples)

## Building from Source

1. Clone the repository:
```bash
git clone https://github.com/signal-slot/qtmcp.git
cd qtmcp
```

2. Configure and build:
```bash
cmake -S . -B build -G Ninja \
      -DCMAKE_PREFIX_PATH=/path/to/qt \
      -DQT_BUILD_EXAMPLES=ON \
      -DQT_BUILD_TESTS=ON
cmake --build build --parallel
```

3. Optionally run the tests and install:
```bash
ctest --test-dir build --output-on-failure
cmake --install build
```

## Getting Started

### A minimal server

A tool is just a `Q_INVOKABLE` method on your `QMcpServer` subclass; the
input schema is generated from the method's signature:

```cpp
#include <QtMcpServer/QMcpServer>

class McpServer : public QMcpServer
{
    Q_OBJECT
public:
    explicit McpServer(const QString &backend = "stdio", QObject *parent = nullptr)
        : QMcpServer(backend, parent) {}

    Q_INVOKABLE QString echo(const QString &message) const {
        return message;
    }

    QHash<QString, QString> toolDescriptions() const override {
        return { { "echo"_L1, "Echoes back the input"_L1 }
               , { "echo/message"_L1, "Message to echo"_L1 } };
    }
};

// stdio:
McpServer server("stdio");
server.start();

// or Streamable HTTP:
McpServer server("streamablehttp");
server.start("127.0.0.1:8000");
```

Tools returning `QFuture` run asynchronously; returning `QImage` produces
image content. See `examples/mcpserver/` for complete programs.

### A minimal client

```cpp
#include <QtMcpClient/QMcpClient>
#include <QtMcpCommon>

QMcpClient client("stdio");
client.start("npx -y @modelcontextprotocol/server-everything");

QMcpInitializeRequest request;
client.request(request, [&](const QMcpInitializeResult &result,
                            const QMcpJSONRPCErrorError *error) {
    // negotiated revision: result.protocolVersion()
    client.notify(QMcpInitializedNotification());
});
```

On 2026-07-28 the initialize handshake is no longer part of the protocol;
requests carry the protocol version in their `_meta` automatically and a
server session initializes itself on first contact.

### Examples

- `examples/mcpclient/inspector/` — GUI client for exploring servers: connect
  over any transport, pick a protocol revision, list/call tools, browse
  resources and prompts.
- `examples/mcpserver/echo/` — minimal stdio/HTTP echo server.
- `examples/mcpserver/window/` — exposes screenshots and cursor control as
  tools (QImage results).
- `examples/mcpserver/texteditor/` — a QMainWindow text editor controlled
  via MCP.

## Features

### Server
- Tools from `Q_INVOKABLE` methods (sync or `QFuture`-async) with generated
  JSON schemas, plus resources, resource templates, prompts and subscriptions
- Per-session protocol revision negotiation, including the 2026-07-28
  stateless lifecycle and `server/discover`
- Change notifications, gated by `subscriptions/listen` opt-ins on
  2026-07-28 (tagged with the subscription id)
- Elicitation (`QMcpServerSession::elicit()`, 2025-06-18 – 2025-11-25) and
  MRTR (`requireInput()`, 2026-07-28)
- Tasks extension for long-running tool calls

### Client
- Typed request/response API over any transport
- Automatic version negotiation and `MCP-Protocol-Version` header handling
- Server-initiated requests (sampling, elicitation) via registered handlers,
  or the `inputRequired` signal on 2026-07-28
- `x-mcp-header` parameter mirroring on Streamable HTTP

### Protocol machinery
- All 150+ protocol types as implicitly shared Qt gadgets with
  reflection-based JSON serialization
- Revision-aware serialization: one type definition serves every revision;
  fields that do not exist in the negotiated revision are dropped on output
  and ignored on input

## Project Structure

```
.
├── src/
│   ├── mcpcommon/      # Protocol types and revision-aware serialization
│   ├── mcpclient/      # QMcpClient
│   ├── mcpserver/      # QMcpServer, QMcpServerSession
│   └── plugins/
│       ├── mcpclientbackend/  # stdio / sse / streamablehttp
│       └── mcpserverbackend/  # stdio / sse / streamablehttp
├── examples/
├── tests/auto/         # Unit and integration tests
├── spec/               # Official MCP schemas (all revisions)
└── docs/               # Implementation plans and notes
```

## Testing

```bash
ctest --test-dir build --output-on-failure
```

The suite covers per-type serialization round trips for every revision,
version negotiation, the stateless 2026-07-28 lifecycle, both Streamable
HTTP generations, MRTR round trips and task polling. CI runs on Linux,
macOS and Windows against Qt 6.8 and the latest Qt release.

## License

Copyright (C) 2025 Signal Slot Inc.
Licensed under:
- LGPL-3.0-only OR
- GPL-2.0-only OR
- GPL-3.0-only

## Contributing

Issues and pull requests are welcome on
[GitHub](https://github.com/signal-slot/qtmcp). For commercial support,
contact [Signal Slot Inc.](https://signal-slot.co.jp/)
