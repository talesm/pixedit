//
// Created by talesm on 13/02/2026.
//
#include "dbActions.hpp"
#include <nlohmann/json.hpp>

namespace pixedit::persist {

static const char createCommand[] = R"==(
PRAGMA foreign_keys = OFF;
DROP TABLE IF EXISTS "Meta";
DROP TABLE IF EXISTS "Command";
DROP TABLE IF EXISTS "History";
PRAGMA foreign_keys = ON;
CREATE TABLE "Meta" (
	"key"	TEXT UNIQUE,
	"value"	INTEGER,
	PRIMARY KEY("key") ON CONFLICT REPLACE
);
CREATE TABLE "History" (
        "id"            INTEGER PRIMARY KEY,
        "description"   TEXT
);
CREATE TABLE "Command" (
	"id"	        INTEGER PRIMARY KEY,
        "history_id"    INTEGER REFERENCES "History"("id") ON UPDATE CASCADE ON DELETE CASCADE,
	"description"	TEXT NOT NULL,
	"options"	TEXT,
	"content"	BLOB
);
INSERT INTO "Meta" VALUES ('format.version', '0.0.1');
INSERT INTO "History" VALUES (NULL, 'BASELINE');
)==";

void
createOrClear(SQLite::Database& db)
{ db.exec(createCommand); }

}
