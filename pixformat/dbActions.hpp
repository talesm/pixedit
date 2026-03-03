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
 * @return the latest versionId.
 */
Sint64
getLatestVersion(SQLite::Database& db);

/**
 * Get all kinds present in a version
 *
 * @param db the database
 * @param versionId the version. If zero or not present get lastest version
 * @return the set with all path kinds.
 */
std::set<std::string>
getKinds(SQLite::Database& db, Sint64 versionId);

/**
 * Create new version
 * @param db the database
 * @param description the description
 * @param versionId the command id to base version into
 * @return the new version command id
 */
Sint64
newVersion(SQLite::Database& db,
           const std::string& description,
           Sint64 versionId = 0);

std::string
ctos(SDL::Color color);

Sint64
putSurface(SQLite::Database& db, Sint64 pos, const Surface& surface);

Sint64
putSurface(SQLite::Database& db,
           Sint64 pos,
           const SDL::Point& size,
           SDL::Color color);

void
getSurface(SQLite::Database& db,
           Sint64 versionId,
           Sint64 pos,
           Surface* surface);

inline Surface
getSurface(SQLite::Database& db, Sint64 versionId, Sint64 pos)
{
  Surface s;
  getSurface(db, versionId, pos, &s);
  return s;
}

struct Layer
{
  Sint64 surface;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Layer, surface)

struct Frame
{
  std::vector<Layer> layers;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Frame, layers)

struct Picture
{
  std::vector<Frame> frames;
  int width;
  int height;
  int depth = 4;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Picture, frames, width, height, depth)

Sint64
putPicture(SQLite::Database& db, Sint64 pos, const Picture& picture);

Picture
getPicture(SQLite::Database& db, Sint64 versionId, Sint64 pos);

}

#endif /* PIXEDIT_PIXFORMAT_INCLUDE_DB_ACTIONS_INCLUDED */
