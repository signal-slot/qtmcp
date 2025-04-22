# Qt MCP (Model Context Protocol)

Qt MCP is a Qt-based implementation of the Model Context Protocol, providing a robust framework for communication between clients and servers in model-driven applications. This library is part of the Qt framework, offering seamless integration with Qt applications.

## Table of Contents
- [Overview](#overview)
- [Requirements](#requirements)
- [Project Structure](#project-structure)
- [Features](#features)
- [Building from Source](#building-from-source)
- [Getting Started](#getting-started)
- [Development](#development)
- [Testing](#testing)
- [Protocol Specification](#protocol-specification)
- [License](#license)
- [Contributing](#contributing)

## Overview

The Model Context Protocol (MCP) defines a standardized way for applications to communicate with language models and other AI services. This Qt implementation provides:

- Client-server communication framework
- Resource management
- Tool execution capabilities
- Prompt template handling
- Real-time notifications
- Progress tracking
- Logging facilities
- Experimental features support

## Requirements

- Qt 6.8.1 or later
- CMake 3.16.0 or later
- C++20 compatible compiler

### Required Qt Components
- Qt Core
- Qt BuildInternals

### Optional Qt Components
- Qt Gui (for Image support)
- Qt Widgets (for GUI examples)
- Qt Network (for HTTP-based communication)

## Project Structure

```
.
├── src/
│   ├── mcpclient/      # Client implementation
│   ├── mcpcommon/      # Shared components and protocol definitions
│   ├── mcpserver/      # Server implementation
│   └── plugins/        # Plugin implementations
│       ├── mcpclientbackend/  # Client backend plugins
│       └── mcpserverbackend/  # Server backend plugins
├── examples/           # Example implementations
│   ├── mcpclient/      # Client examples
│   │   └── inspector/  # MCP Inspector GUI
│   └── mcpserver/      # Server examples
│       ├── echo/       # Echo server example
│       └── window/     # Window server example
├── tests/              # Test suite
└── spec/              # Protocol specification
    └── schema.json    # JSON Schema definition
```

## Features

### Client Capabilities
- Resource access and management
- Tool execution with schema validation
- Progress tracking and cancellation
- Prompt template handling
- Real-time notifications
- Customizable logging levels
- Experimental features support
- Sampling capabilities

### Server Capabilities
- Resource provision and template support
- Tool hosting with input validation
- Prompt template management
- Progress reporting and cancellation
- Logging facilities with severity levels
- Subscription management
- HTTP server capabilities
- Experimental features

### Protocol Features
- JSON-RPC based communication
- URI-based resource identification
- Template-based resource access
- Tool execution framework with schema validation
- Progress notification system with cancellation support
- Logging with configurable severity levels
- Experimental features framework
- Sampling and model hints

## Building from Source

1. Clone the repository:
```bash
git clone https://gitlab.com/signal-slot/qtmcp.git
cd qtmcp
```

2. Create a build directory:
```bash
mkdir build && cd build
```

3. Configure with CMake:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/qt \
      -DBUILD_EXAMPLES=ON \
      -DBUILD_TESTING=ON ..
```

4. Build the project:
```bash
cmake --build . --parallel
```

5. Optionally run tests:
```bash
ctest --output-on-failure
```

## Getting Started

The project includes example applications that demonstrate both client and server implementations of the MCP protocol:

### MCP Inspector (Client Example)
Located in `examples/mcpclient/inspector/`, the MCP Inspector provides a comprehensive GUI for:
- Connection management with MCP servers
- Tool execution and monitoring
- Resource browsing and management
- Prompt template listing and execution
- Real-time protocol inspection
- Experimental features testing

Example usage:
```cpp
#include <QMcpClient>

// Create and configure the client
QMcpClient client;
client.setServerUrl(QUrl("mcp://localhost:8080"));

// Connect to server
client.connectToServer();

// Execute a tool
QJsonObject args;
args["input"] = "Hello, World!";
client.callTool("echo", args);
```

### Echo Server Example
Located in `examples/mcpserver/echo/`, this example demonstrates:
- Basic MCP server implementation
- Standard input/output communication
- Simple request-response pattern
- Resource template usage

Example implementation:
```cpp
#include <QMcpServer>

class EchoServer : public QMcpServer
{
    Q_OBJECT
public:
    void handleToolCall(const QString &tool, const QJsonObject &args) override
    {
        if (tool == "echo") {
            emit toolResult(args["input"].toString());
        }
    }
};
```

### Window Server Example
Located in `examples/mcpserver/window/`, this example shows:
- GUI-based MCP server implementation
- Window management capabilities
- Complex resource handling
- Tool implementation patterns

### Building the Examples

```bash
mkdir build && cd build
cmake -DBUILD_EXAMPLES=ON ..
cmake --build . --parallel
```

The example executables will be available in the build directory under their respective paths.

## Development

### Plugin System
Qt MCP supports a plugin-based architecture for both client and server backends:

- Client Plugins (`src/plugins/mcpclientbackend/`):
  - Implement `QMcpClientBackendInterface`
  - Handle client-side communication
  - Support different transport mechanisms
  - Experimental features support

- Server Plugins (`src/plugins/mcpserverbackend/`):
  - Implement `QMcpServerBackendInterface`
  - Handle server-side communication
  - Support custom resource and tool implementations
  - HTTP server capabilities

### CMake Build System
The project uses a modular CMake build system:
- `.cmake.conf`: Global configuration variables
- `configure.cmake`: Build system configuration
- `qt_cmdline.cmake`: Command-line build options
- Component-specific configuration in `src/*/configure.cmake`

### Adding New Tools
Tools can be implemented by creating a new tool definition that follows the MCP schema. The tool definition includes:
- Name and description
- Input schema specification (JSON Schema)
- Implementation of the tool's functionality
- Optional experimental features

Example tool definition:
```cpp
QMcpTool tool;
tool.setName("calculator");
tool.setDescription("Performs basic arithmetic operations");
tool.setInputSchema(R"({
    "type": "object",
    "properties": {
        "operation": {
            "type": "string",
            "enum": ["add", "subtract", "multiply", "divide"]
        },
        "operands": {
            "type": "array",
            "items": { "type": "number" },
            "minItems": 2
        }
    },
    "required": ["operation", "operands"]
})");
```

### Resource Management
Resources are identified by URIs and can be:
- Static resources with direct URIs
- Dynamic resources using URI templates
- Blob and text content types
- Subscribed to for real-time updates

### Error Handling
The protocol provides comprehensive error handling through:
- Standard error codes
- Detailed error messages
- Progress notifications with cancellation
- Logging with configurable severity levels
- JSON-RPC error responses

## Testing

The project includes a comprehensive test suite:

- Unit Tests: Located in `tests/auto/`
  - Client tests (`tests/auto/mcpclient/`)
    - QMcpClient functionality
    - Backend interface testing
    - Plugin system verification
  - Common component tests (`tests/auto/mcpcommon/`)
  - Server tests (`tests/auto/mcpserver/`)

Run the tests using:
```bash
cd build
ctest --output-on-failure
```

## Protocol Specification

The Model Context Protocol is defined using JSON Schema. Key components include:

- Message Types:
  - Requests (client to server and vice versa)
  - Notifications (asynchronous events)
  - Results (responses to requests)

- Core Concepts:
  - Resources: Identified by URIs, can be static or template-based
  - Tools: Server-provided functions with defined input schemas
  - Prompts: Template-based message generation
  - Roles: User and assistant roles in communication
  - Experimental Features: Optional protocol extensions

### Specification Status

Qt MCP currently has two specification versions:

- **2024-11-05**: The stable version that is fully implemented and deployed in production.
- **2025-03-26**: The newer version with several enhancements that has completed core implementation and is now in testing phase.

Key differences in the 2025-03-26 specification include:

- Audio Content Support: New `AudioContent` type for audio data handling ✅ **Implemented**
- Annotations Structure Change: Direct references to `Annotations` instead of inheritance from `Annotated` ✅ **Implemented**
- JSON-RPC Batch Support: New types for batch requests and responses ✅ **Implemented**
- Model Hints: Enhanced model preference specification ✅ **Implemented**

The implementation maintains backward compatibility with the 2024-11-05 schema while adding support for all new features in the 2025-03-26 schema. The core implementation is complete, and the project is currently in the testing phase. For the detailed status and remaining tasks, see the update plan outlined in `update-plan-mcp-2025-03-26.md`.

Schema Structure Example:
```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "definitions": {
    "Request": {
      "type": "object",
      "properties": {
        "jsonrpc": { "const": "2.0" },
        "method": { "type": "string" },
        "params": { "type": "object" },
        "id": { "type": ["string", "number"] }
      },
      "required": ["jsonrpc", "method", "id"]
    },
    "Response": {
      "type": "object",
      "properties": {
        "jsonrpc": { "const": "2.0" },
        "result": { "type": "object" },
        "error": { "$ref": "#/definitions/Error" },
        "id": { "type": ["string", "number"] }
      },
      "required": ["jsonrpc", "id"]
    }
  }
}
```

For detailed specification information, see:
- `spec/schema-2024-11-05.json`: Current implementation
- `spec/schema-2025-03-26.json`: Upcoming implementation

## License

Copyright (C) 2025 Signal Slot Inc.
Licensed under:
- LGPL-3.0-only OR
- GPL-2.0-only OR
- GPL-3.0-only

## Contributing

For more information about contributing to Qt MCP, please visit [Signal Slot's website](https://signal-slot.co.jp/).
