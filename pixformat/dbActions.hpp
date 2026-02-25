//
// Created by talesm on 13/02/2026.
//

#ifndef PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED
#define PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED

#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>

#include "Surface.hpp"

namespace pixedit::persist {

using json = nlohmann::json;

/**
 * Create or clear database tables.
 *
 * @param db the database to apply to.
 */
void
createOrClear(SQLite::Database& db);

/**
 * Create or clear database tables.
 *
 * @param db the database to apply to.
 */
void
createOrClear(SQLite::Database& db,
              const SDL::Point& size,
              SDL::Color color = {});

/**
 * Create single frame single layer picture.
 *
 * @param db the database.
 * @param surface surface.
 */
void
createOrClear(SQLite::Database& db, const Surface& surface);

Sint64
insertResource(SQLite::Database& db, const Surface& surface);

Sint64
insertResource(SQLite::Database& db,
               const json& options,
               std::span<Uint8> content);

Sint64
insertResource(SQLite::Database& db, const json& options, Sint64 bufferId = 0);

Sint64
insertAction(SQLite::Database& db, Sint64 resourceId, const std::string& kind);

std::string
ctos(SDL::Color color);

}

#endif /* PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED */
