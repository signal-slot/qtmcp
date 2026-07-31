# MCP 全プロトコルリビジョン対応計画

対象: 2024-11-05(対応済)/ 2025-03-26(対応済)/ **2025-06-18 / 2025-11-25 / 2026-07-28(本計画で追加)**

方針: 既存の「`toJsonObject`/`fromJsonObject` に protocolVersion を渡し、セッション毎のネゴシエート済みバージョンで分岐」する機構
(mcp-version-annotations-support-plan-final.md)を拡張する。段階的に 1 リビジョンずつ実装し、各段階でテストを追加してからコミットする。

## Phase 1: 2025-06-18

schema 差分: +BaseMetadata, BooleanSchema, ContentBlock, ElicitRequest, ElicitResult, EnumSchema,
NumberSchema, PrimitiveSchemaDefinition, ResourceLink, ResourceTemplateReference, StringSchema
/ -JSONRPCBatchRequest, JSONRPCBatchResponse, ResourceReference(改名)

ワイヤーに影響する変更:

1. `QtMcp::ProtocolVersion` に `v2025_06_18` を追加、文字列変換・ネゴシエーションを更新
2. structured tool output: `QMcpTool` に `outputSchema`、`QMcpCallToolResult` に `structuredContent`
3. ContentBlock 化: `QMcpResourceLink` 新設、ツール結果コンテンツの union に追加
4. elicitation: `QMcpElicitRequest`/`QMcpElicitResult` + primitive schema 型一式、
   `QMcpClientCapabilities` に `elicitation`
5. `title` フィールド(BaseMetadata): Tool/Prompt/Resource/ResourceTemplate/Implementation 等
6. `_meta` の追加対象拡大
7. `QMcpCompleteRequestParams` に `context`、`ResourceReference` → `ResourceTemplateReference`(改名。
   → 実装時確認: ワイヤー上は両者とも `"type": "ref/resource"` で同一のためコード変更不要)
8. JSON-RPC batching をこのバージョン以降で拒否(受信時エラー)
   → 実装時確認: qtmcp は batch 型が存在するだけでディスパッチ未配線のため、拒否処理は不要
9. HTTP トランスポート: `MCP-Protocol-Version` ヘッダの送出(クライアント)/検証(サーバー)
   → クライアント SSE バックエンドで送出を実装。サーバー側検証は Streamable HTTP
   トランスポート実装(下記「横断課題」)まで保留(旧 HTTP+SSE はヘッダ要件より前に廃止された転送のため)

## Phase 2: 2025-11-25

schema 差分(ワイヤー影響分):

1. `ProtocolVersion` に `v2025_11_25` を追加
2. icons: `QMcpIcon`/`QMcpIcons` 新設、Tool/Prompt/Resource/Implementation に付与
3. `QMcpImplementation` に `description`
4. elicitation 拡張: enum schema の再設計(titled/untitled × single/multi-select)、
   URL mode elicitation(`mode`, `url`, `elicitationId`)、`notifications/elicitation/complete`、
   デフォルト値対応
5. sampling へのツール呼び出し対応: `tools`/`toolChoice` パラメータ、
   `QMcpToolUseContent`/`QMcpToolResultContent`
6. tasks(experimental): `tasks/create` 系メソッド、`QMcpTask`、`QMcpTaskStatusNotification` 等
   → 決定: **型のみ実装**し、QMcpServer/QMcpClient のランタイム配線(タスク永続化・
   ポーリング応答)は保留。2026-07-28 で extension 化・再設計されるため、ランタイムは
   extension 対応(Phase 3 以降)で扱う
7. schema 上の standalone *Params 分離はワイヤー不変のため型追加のみで対応可
8. → 実装メモ: URL mode elicitation は Form/URL の union ではなくフラットな
   `QMcpElicitRequestParams` に統合し、`mode` を判別子として `isPropertyAvailable` を
   mode 依存に拡張(fromJsonObject が mode を先読み)。enum schema の新変種 4 型は
   `QMcpPrimitiveSchemaDefinition` の判別ロジックに統合し、既存 `QMcpEnumSchema` は
   LegacyTitledEnumSchema 相当として維持。`QMcpAnyOf::fromJsonObject` は変種自身の
   fromJsonObject への委譲に簡素化(バージョンゲート・キー変換が union 内でも効くように)

## Phase 3: 2026-07-28

アーキテクチャ変更を伴う。既存バージョンとの共存が最大の設計課題。

1. `ProtocolVersion` に `v2026_07_28` を追加
2. **ステートレス化**: このバージョンでは initialize/initialized を使わない。
   毎リクエストの `_meta` に `io.modelcontextprotocol/protocolVersion`・`clientCapabilities`・
   `clientInfo` を載せ、レスポンス `_meta` に `serverInfo` を返す
3. **`server/discover`**: サーバー側は実装必須(MUST)。クライアント側は STDIO の後方互換プローブとして使用
4. **`resultType` 必須化**: 全 Result 型に `resultType`("complete" / "input_required")。
   旧バージョンでは出力しない
5. **MRTR**: `InputRequiredResult`(`inputRequests`)+ リトライ時の `inputResponses`。
   サーバー発リクエスト(roots/list, sampling/createMessage, elicitation/create)はこの
   バージョンでは MRTR パターンに変換
6. **`subscriptions/listen`**: HTTP GET ストリームと `resources/subscribe`/`unsubscribe` を置換。
   opt-in 種別(toolsListChanged 等)、`io.modelcontextprotocol/subscriptionId` タグ付け
7. **削除**: `ping`, `logging/setLevel`, `notifications/roots/list_changed`(このバージョンのみ)。
   ログレベルは `_meta` の `io.modelcontextprotocol/logLevel`
8. **CacheableResult**: `tools/list` 等の結果に `ttlMs`/`cacheScope` 必須
9. エラーコード: resource not found -32002→-32602、`UnsupportedProtocolVersionError` 等の新設、
   -32020〜-32099 の MCP 予約帯
10. HTTP: `Mcp-Session-Id` 廃止、`Mcp-Method`/`Mcp-Name` ヘッダ必須、SSE resumability 廃止

### Phase 3 実装メモ

- ステートレス化: サーバーは params `_meta` の `io.modelcontextprotocol/protocolVersion` で
  セッションを自動初期化(initialize は旧版互換として存続)。クライアントは 2026-07-28 時に
  全リクエストへ `_meta`(protocolVersion / clientInfo)を注入し、応答 `_meta` に serverInfo を返す
- `resultType` は `QMcpResult` 基底で実装(REQUIRED + バージョンゲート、欠落受信は "complete" 扱い)。
  `ttlMs`/`cacheScope` は各 list Result に同パターンで実装
- `subscriptions/listen`: opt-in フィルタと subscriptionId を `QMcpServerSession` に保持し、
  2026-07-28 セッションの変更通知はフィルタ通過分のみ `_meta` タグ付きで送出。
  専用ストリームの概念は stdio/旧 SSE では通常の通知フローに縮退(Streamable HTTP 実装時に再訪)
- MRTR: 型(`QMcpInputRequiredResult` 等)+ クライアントの `inputRequired` シグナル
  (interim result 検出、リトライはアプリ責務)まで実装。サーバー側でツールハンドラが
  input_required を返す高レベル API と、roots/sampling/elicitation の MRTR 変換は**未実装**
  (フォローアップ)。2026-07-28 セッションでの `elicit()`/`createMessage()` は警告して送信しない
- リソース不在エラーコード -32002→-32602: qtmcp は -32002 を使っていなかったため対応不要
- 2025-11-25 の experimental tasks 同様、tasks extension(`io.modelcontextprotocol/tasks`)は未実装

## 横断課題

- **Streamable HTTP トランスポート**: Phase 4 で実装済み(プラグインキー "streamablehttp"、
  サーバー/クライアント両対応。2025-03-26〜11-25 の Mcp-Session-Id/GET ストリーム/DELETE と
  2026-07-28 のセッションレス+ヘッダ検証+listen ストリームの両世代に対応)。
  残 TODO(コード内に明記): 2026-07-28 の -32601 は HTTP 404 で返すべき、
  Last-Event-ID 再開は未提供、不正な x-mcp-header 注釈はツール除外でなく警告のみ
- **サーバー側 MRTR**: Phase 5 で実装済み(`QMcpServerSession::requireInput()` +
  requestState ラウンドトリップでサーバーはステートレス維持)
- **tasks extension**: Phase 6 で実装済み(型 + サーバーレジストリ + tasks/get/cancel/update、
  discover での広告、クライアント宣言 API)。タスク中の input_required とツール再開の
  自動接続は tasks/update での保存まで(アプリ責務)
- バージョン分岐の表現: `QMcpGadget::isPropertyAvailable(name, version)` を導入済み。
  enum 値は日付順の数値なので `version >= v2025_06_18` の順序比較で分岐する
- 型の追加はすべて追加的(既存型の削除はしない)。旧バージョンで存在しないフィールドは
  シリアライズ時に落とす
- spec/ に新 schema 3 本を追加
- テスト: 各バージョンのクライアント⇔サーバー往復テスト、バージョン混在セッションテスト
