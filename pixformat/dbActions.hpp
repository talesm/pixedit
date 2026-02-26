//
// Created by talesm on 13/02/2026.
//

#ifndef PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED
#define PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED

#include <set>

#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>

#include "Surface.hpp"

namespace pixedit::persist {

using json = nlohmann::json;

/**
 * Create single frame single layer picture.
 *
 * @param db the database.
 * @param surface surface.
 */
void
createOrClear(SQLite::Database& db, const Surface& surface);

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
 * Get the latest version
 *
 * @param db the database
 * @return the latest command_id.
 */
Sint64
getLatestVersion(SQLite::Database& db);

/**
 * Get all kinds present in a version
 *
 * @param db the database
 * @param command_id the version. If zero or not present get lastest version
 * @return the set with all path kinds.
 */
std::set<std::string>
getKinds(SQLite::Database& db, Sint64 command_id);

std::string
ctos(SDL::Color color);

Sint64
putSurface(SQLite::Database& db, Sint64 pos, const Surface& surface);

Sint64
putSurface(SQLite::Database& db,
           Sint64 pos,
           const SDL::Point& size,
           SDL::Color color);

}

#endif /* PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED */
