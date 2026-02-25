//
// Created by talesm on 13/02/2026.
//
#include "dbActions.hpp"
#include <doctest/doctest.h>
#include <picosha2.h>

namespace pixedit::persist {

static const char createCommand[] = R"==(
SAVEPOINT "Clearing";
PRAGMA foreign_keys = OFF;
DROP TABLE IF EXISTS "Action";
DROP TABLE IF EXISTS "Path";
DROP TABLE IF EXISTS "Command";
DROP TABLE IF EXISTS "Resource";
DROP TABLE IF EXISTS "Buffer";
DROP TABLE IF EXISTS "Meta";
PRAGMA foreign_keys = ON;
CREATE TABLE "Meta" (
	"key"	TEXT UNIQUE,
	"value"	TEXT,
	PRIMARY KEY("key") ON CONFLICT REPLACE
);
CREATE TABLE "Buffer" (
        "id"            INTEGER PRIMARY KEY,
        "hash"          BLOB NOT NULL,
	"content"	BLOB UNIQUE
);
CREATE TABLE "Resource" (
        "id"            INTEGER PRIMARY KEY,
	"options"	TEXT DEFAULT '{}',
        "buffer_id"     INTEGER REFERENCES "Buffer"("id")
);
CREATE TABLE "Command" (
        "id"            INTEGER PRIMARY KEY,
        "description"   TEXT NOT NULL,
	"options"       TEXT DEFAULT '{}'
);
CREATE TABLE "Path" (
        "id"            INTEGER PRIMARY KEY,
        "kind"          TEXT NOT NULL,
        "pos"           INTEGER NOT NULL,
        UNIQUE (kind, pos)
);
CREATE TABLE "Action" (
	"id"	        INTEGER PRIMARY KEY,
	"resource_id"   INTEGER REFERENCES "Image"("id") ON UPDATE CASCADE ON DELETE CASCADE,
        "command_id"    INTEGER NOT NULL REFERENCES "Command"("id") ON UPDATE CASCADE ON DELETE CASCADE,
        "path_id"       INTEGER NOT NULL REFERENCES "Path"("id") ON UPDATE CASCADE ON DELETE CASCADE,
        UNIQUE (command_id, path_id) ON CONFLICT REPLACE
);
INSERT INTO "Meta" VALUES ('format.version', '0.0.1');
INSERT INTO "Command" VALUES (1, '', '{"baseline": true}');
RELEASE SAVEPOINT "Clearing";
)==";

void
createOrClear(SQLite::Database& db)
{ db.exec(createCommand); }

static void
copyTo(const Surface& surface, Uint8* target);

static Sint64
insertBuffer(SQLite::Database& db,
             std::span<Uint8> hash,
             std::span<Uint8> buffer = {})
{
  // Prepare query
  SQLite::Statement query{
    db,
    R"===(INSERT INTO "Buffer" ("hash", "content") VALUES (?, ?) RETURNING id;)==="};

  // Bind values
  query.bindNoCopy(1, hash.data(), hash.size_bytes());
  if (buffer.empty()) {
    query.bind(2);
  } else {
    query.bindNoCopy(2, buffer.data(), buffer.size_bytes());
  }

  // Exec
  if (!query.executeStep()) throw std::runtime_error{"Error creating buffer"};
  return query.getColumn(0).getInt64();
}

static std::vector<Uint8>
makeHash(std::span<Uint8> buffer)
{
  std::vector<Uint8> hash(picosha2::k_digest_size);
  picosha2::hash256(buffer.begin(), buffer.end(), hash.begin(), hash.end());
  return hash;
}

static Sint64
makeBuffer(SQLite::Database& db, std::span<Uint8> buffer)
{
  if (buffer.size_bytes() <= 64) {
    // Prepare query
    SQLite::Statement query(
      db,
      R"===(SELECT "id" FROM "Buffer" WHERE "hash" = ? AND "content" IS NULL)===");

    query.bind(1, buffer.data(), buffer.size());
    if (query.executeStep()) { return query.getColumn(0).getInt64(); }

    return insertBuffer(db, buffer);
  }
  // Prepare query
  SQLite::Statement query(
    db,
    R"===(SELECT "id", "content" FROM "Buffer" WHERE "hash" = ? AND "content" IS NOT NULL)===");

  auto hash = makeHash(buffer);
  query.bind(1, hash.data(), hash.size());

  while (query.executeStep()) {
    auto blobColumn = query.getColumn(1);
    auto size = blobColumn.size();
    if (size != buffer.size()) continue;
    auto blob = blobColumn.getBlob();
    if (SDL::memcmp(blob, buffer.data(), size) == 0) {
      return query.getColumn(0).getInt64();
    }
  }

  return insertBuffer(db, hash, buffer);
}

TEST_CASE("makeBuffer")
{
  SQLite::Database db("", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  createOrClear(db);

  SUBCASE("Small")
  {
    Uint8 buffer1[4] = {1, 3, 3, 7};
    auto id1 = makeBuffer(db, buffer1);
    REQUIRE(id1 != 0);

    Uint8 buffer2[8] = {1, 3, 3, 7, 42, 64};
    auto id2 = makeBuffer(db, buffer2);
    REQUIRE(id2 != 0);
    REQUIRE(id1 != id2);

    auto id3 = makeBuffer(db, buffer1);
    REQUIRE(id3 != 0);
    REQUIRE(id3 == id1);
  }
  SUBCASE("Big")
  {
    Uint8 buffer1[128] = {1, 3, 3, 7};
    auto id1 = makeBuffer(db, buffer1);
    REQUIRE(id1 != 0);

    Uint8 buffer2[128] = {1, 3, 3, 7, 42, 64};
    auto id2 = makeBuffer(db, buffer2);
    REQUIRE(id2 != 0);
    REQUIRE(id1 != id2);

    auto id3 = makeBuffer(db, buffer1);
    REQUIRE(id3 != 0);
    REQUIRE(id3 == id1);
  }
}

static std::vector<Uint8>
makeSurface32Buffer(const Surface& surface)
{
  std::vector<Uint8> buffer;
  buffer.resize(surface->w * surface->h * 4);
  if (surface.GetFormat() != SDL::PIXELFORMAT_ABGR32) {
    copyTo(surface.Convert(SDL::PIXELFORMAT_ABGR32), buffer.data());
  } else {
    copyTo(surface, buffer.data());
  }
  return buffer;
}

Sint64
insertResource(SQLite::Database& db, const Surface& surface)
{
  // Copy data to buffer
  int width = surface.GetWidth();
  int height = surface.GetHeight();
  int depth = 4;

  auto content = makeSurface32Buffer(surface);
  const auto bufferId = makeBuffer(db, content);

  // Store options
  json options{
    {"width", width}, {"height", height}, {"depth", depth}}; // Prepare query
  return insertResource(db, options, bufferId);
}

Sint64
insertResource(SQLite::Database& db,
               const json& options,
               std::span<Uint8> content)
{
  const auto bufferId = insertBuffer(db, content);
  return insertResource(db, options, bufferId);
}

Sint64
insertResource(SQLite::Database& db, const json& options, Sint64 bufferId)
{
  SQLite::Statement query{
    db,
    R"===(INSERT INTO "Resource" (buffer_id, options) VALUES (?, ?) RETURNING id;)==="};

  // Bind values
  query.bind(1, bufferId);
  if (bufferId != 0) {
    query.bind(2, options.dump());
  } else {
    query.bind(2);
  }

  // Exec
  if (!query.executeStep()) throw std::runtime_error{"Error creating resource"};
  return query.getColumn(0).getInt64();
}

std::string
ctos(SDL::Color color)
{
  return std::format("{:02x}{:02x}{:02x}{:02x}",
                     int(color.r),
                     int(color.g),
                     int(color.b),
                     int(color.a));
}

void
createOrClear(SQLite::Database& db, const SDL::Point& size, SDL::Color color)
{
  createOrClear(db);
  // Prepare query
  const auto imageId = insertResource(db,
                                      json{
                                        {"width", size.x},
                                        {"height", size.y},
                                        {"color", ctos(color)},
                                      });
  insertAction(db, imageId, "image");

  db.exec(R"===(INSERT INTO "Meta" VALUES ('current.mode', 'image');)===");
  db.exec(R"===(INSERT INTO "Meta" VALUES ('current.image', 1);)===");
}

TEST_CASE("CreateOrClearFromColor")
{
  SQLite::Database db("", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
  createOrClear(db, {8, 8}, {1, 2, 3, 4});

  auto currentMode =
    db.execAndGet("SELECT value FROM Meta WHERE key = 'current.mode'")
      .getString();
  REQUIRE(currentMode == "image");

  auto currentPicture =
    db.execAndGet("SELECT value FROM Meta WHERE key = 'current.image'")
      .getInt();
  REQUIRE(currentPicture == 1);
}

static Sint64
insertPath(SQLite::Database& db, const std::string& kind)
{
  SQLite::Statement query{db, R"===(INSERT INTO "Path" (kind, pos) VALUES (?1,
    IFNULL((SELECT MAX(pos) FROM "Path" WHERE kind = ?1), 0) + 1) RETURNING id;)==="};
  // Exec
  query.bind(1, kind);
  if (!query.executeStep()) throw std::runtime_error{"Error creating path"};
  return query.getColumn(0).getInt64();
}

Sint64
insertAction(SQLite::Database& db, Sint64 resourceId, const std::string& kind)
{
  const auto pathId = insertPath(db, kind);

  SQLite::Statement query{
    db, R"===(INSERT INTO "Action" (resource_id, command_id, path_id)
VALUES (?, (SELECT MAX(id) FROM "Command"), ?) RETURNING id;)==="};

  // Bind values
  query.bind(1, resourceId);
  query.bind(2, pathId);

  // Exec
  if (!query.executeStep()) throw std::runtime_error{"Error creating action"};
  return query.getColumn(0).getInt64();
}

void
createOrClear(SQLite::Database& db, const Surface& surface)
{
  createOrClear(db);
  const auto resourceId = insertResource(db, surface);
  insertAction(db, resourceId, "image");

  db.exec(R"===(INSERT INTO "Meta" VALUES ('current.image', 1);)===");
}

static void
copyTo(const Surface& surface, Uint8* target)
{
  auto source = static_cast<const Uint8*>(surface->pixels);
  auto pitch = surface->pitch;
  auto w = surface->w;
  auto h = surface->h;
  for (auto i = 0; i < h; ++i) {
    memcpy(target, source, pitch);
    source += pitch;
    target += w * 4;
  }
}

}
