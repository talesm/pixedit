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
 * Create single frame single layer picture.
 *
 * @param db the database.
 * @param surface surface.
 */
void
createOrClear(SQLite::Database& db, const Surface& surface);

}

#endif /* PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED */
