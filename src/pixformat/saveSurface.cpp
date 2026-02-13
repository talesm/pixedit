//
// Created by talesm on 11/02/2026.
//
#include <SQLiteCpp/SQLiteCpp.h>
#include <nlohmann/json.hpp>
#include "dbActions.hpp"

#include "Surface.hpp"

namespace pixedit {

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

void
savePixSurface(const Surface& surface, const std::string& filename)
{
  SQLite::Database db(filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

  // Begin transaction
  SQLite::Transaction transaction(db);
  // Clear
  persist::createOrClear(db);
  // Prepare query
  SQLite::Statement query{db, R"===(
INSERT INTO "Command" (description, history_id, options, content) VALUES (?, 1, ?, ?);)==="};

  size_t sz = surface.GetWidth() * surface.GetHeight() * 4;
  std::unique_ptr<Uint8[]> content{new Uint8[sz]};
  if (surface.GetWidth() != SDL::PIXELFORMAT_ABGR32) {
    copyTo(surface.Convert(SDL::PIXELFORMAT_ABGR32), content.get());
  } else {
    copyTo(surface, content.get());
  }

  using json = nlohmann::json;
  json command{{"width", surface.GetWidth()},
               {"height", surface.GetHeight()},
               {"depth", 4}};

  // Add image
  query.bind(1, "image.replace");
  query.bind(2, command.dump());
  query.bindNoCopy(3, content.get(), sz);

  query.exec();
  query.reset();

  // Commit transaction
  transaction.commit();
}

}
