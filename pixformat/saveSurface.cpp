//
// Created by talesm on 11/02/2026.
//
#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>
#include "dbActions.hpp"

#include "Surface.hpp"

namespace pixedit {

void
savePixSurface(const Surface& surface, const std::string& filename)
{
  SQLite::Database db(filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

  // Begin transaction
  SQLite::Transaction transaction(db);
  // Clear
  persist::createOrClear(db, surface);

  // Commit transaction
  transaction.commit();
}

}
