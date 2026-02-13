//
// Created by talesm on 13/02/2026.
//

#ifndef PIXEDITOR_DBACTIONS_HPP
#define PIXEDITOR_DBACTIONS_HPP

#include <SQLiteCpp/SQLiteCpp.h>

namespace pixedit::persist {

/**
 * Create or clear database tables.
 *
 * @param db the database to apply to.
 */
void
createOrClear(SQLite::Database& db);

}

#endif // PIXEDITOR_DBACTIONS_HPP
