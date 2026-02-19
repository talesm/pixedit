//
// Created by talesm on 13/02/2026.
//
#include "dbActions.hpp"

namespace pixedit::persist {

static const char createCommand[] = R"==(
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
	"content"	BLOB
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
)==";

void
createOrClear(SQLite::Database& db)
{ db.exec(createCommand); }

static void
copyTo(const Surface& surface, Uint8* target);

static Sint64
insertBuffer(SQLite::Database& db, std::span<Uint8> buffer)
{
  // Prepare query
  SQLite::Statement query{
    db, R"===(INSERT INTO "Buffer" (content) VALUES (?) RETURNING id;)==="};

  // Bind values
  query.bindNoCopy(1, buffer.data(), buffer.size_bytes());

  // Exec
  if (!query.executeStep()) throw std::runtime_error{"Error creating buffer"};
  return query.getColumn(0).getInt64();
}

static Sint64
insertResource(SQLite::Database& db, const Surface& surface)
{
  // Copy data to buffer
  int width = surface.GetWidth();
  int height = surface.GetHeight();
  int depth = 4;

  size_t sz = width * height * depth;
  std::unique_ptr<Uint8[]> content{new Uint8[sz]};
  if (surface.GetFormat() != SDL::PIXELFORMAT_ABGR32) {
    copyTo(surface.Convert(SDL::PIXELFORMAT_ABGR32), content.get());
  } else {
    copyTo(surface, content.get());
  }

  const auto bufferId = insertBuffer(db, std::span{(content.get()), sz});

  SQLite::Statement query{
    db,
    R"===(INSERT INTO "Resource" (buffer_id, options) VALUES (?, ?) RETURNING id;)==="};

  // Store options
  json options{
    {"width", width}, {"height", height}, {"depth", depth}}; // Prepare query

  // Bind values
  query.bind(1, bufferId);
  query.bind(2, options.dump());

  // Exec
  if (!query.executeStep()) throw std::runtime_error{"Error creating resource"};
  return query.getColumn(0).getInt64();
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

static Sint64
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
