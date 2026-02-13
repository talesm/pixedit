//
// Created by talesm on 13/02/2026.
//
#include "dbActions.hpp"

namespace pixedit::persist {

static const char createCommand[] = R"==(
PRAGMA foreign_keys = OFF;
DROP TABLE IF EXISTS "Command";
DROP TABLE IF EXISTS "History";
DROP TABLE IF EXISTS "Image";
DROP TABLE IF EXISTS "Surface";
DROP TABLE IF EXISTS "Meta";
PRAGMA foreign_keys = ON;
CREATE TABLE "Meta" (
	"key"	TEXT UNIQUE,
	"value"	INTEGER,
	PRIMARY KEY("key") ON CONFLICT REPLACE
);
CREATE TABLE "Surface" (
        "id"            INTEGER PRIMARY KEY,
	"options"	TEXT DEFAULT '{}',
	"content"	BLOB
);
CREATE TABLE "Image" (
        "id"            INTEGER PRIMARY KEY,
        "surface_id"    INTEGER REFERENCES "Surface"("id"),
	"options"	TEXT DEFAULT '{}'
);
CREATE TABLE "History" (
        "id"            INTEGER PRIMARY KEY,
        "description"   TEXT NOT NULL,
	"options"       TEXT DEFAULT '{}'
);
CREATE TABLE "Command" (
	"id"	        INTEGER PRIMARY KEY,
        "history_id"    INTEGER REFERENCES "History"("id") ON UPDATE CASCADE ON DELETE CASCADE,
	"surface_id"    INTEGER REFERENCES "Surface"("id"),
	"image_id"      INTEGER REFERENCES "Image"("id"),
        "palette_id"    INTEGER,
	"options"       TEXT DEFAULT '{}'
);
INSERT INTO "Meta" VALUES ('format.version', '0.0.1');
)==";

void
createOrClear(SQLite::Database& db)
{ db.exec(createCommand); }

static void
copyTo(const Surface& surface, Uint8* target);

void
createOrClear(SQLite::Database& db, const Surface& surface)
{
  createOrClear(db);

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

  // Store options
  json options{{"width", width}, {"height", height}, {"depth", depth}};
  // Prepare query
  SQLite::Statement query{
    db, R"===(INSERT INTO "Surface" (options, content) VALUES (?, ?);)==="};

  // Bind values
  query.bind(1, options.dump());
  query.bindNoCopy(2, content.get(), sz);

  // Exec
  query.exec();
  db.exec(R"===(INSERT INTO "Meta" VALUES ('current.image', 1);)===");
  db.exec(R"===(INSERT INTO "Image" (surface_id) VALUES (1);)===");
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
