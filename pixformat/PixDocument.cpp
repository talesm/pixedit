//
// Created by talesm on 24/02/2026.
//

#include "PixDocument.hpp"
#include <doctest/doctest.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "dbActions.hpp"

namespace pixedit {

struct PixDocument::impl
{
  SQLite::Database db;

  impl(SQLite::Database&& db)
    : db{std::move(db)} {};
};

PixDocument::PixDocument() = default;

PixDocument::PixDocument(PixDocument&&) noexcept = default;

PixDocument&
PixDocument::operator=(PixDocument&&) noexcept = default;

PixDocument::~PixDocument() = default;

PixDocument
PixDocument::create(const std::string& filename, const SDL::Point& size)
{
  PixDocument doc(std::make_unique<impl>(
    SQLite::Database(filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)));
  doc.clearContents(size);
  return doc;
}

TEST_CASE("PixDocument.create")
{
  constexpr auto filename = "./test_create_1.db";
  auto doc = PixDocument::create(filename, {8, 8});
  REQUIRE_NOTHROW(SQLite::Database(filename, SQLite::OPEN_READONLY));
}

PixDocument
PixDocument::convert(const std::string& filename, const SDL::Surface& surface)
{
  PixDocument doc(std::make_unique<impl>(
    SQLite::Database(filename, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)));
  doc.clearContents(surface);
  return doc;
}
PixDocument
PixDocument::load(const std::string& filename)
{
  PixDocument doc(
    std::make_unique<impl>(SQLite::Database(filename, SQLite::OPEN_READWRITE)));
  // todo: Validate
  return doc;
}

void
PixDocument::clearContents(const SDL::Point& size)
{
  SQLite::Transaction transaction(pimpl->db);
  persist::createOrClear(pimpl->db, size);
  transaction.commit();
}

void
PixDocument::clearContents(const SDL::Surface& surface)
{
  SQLite::Transaction transaction(pimpl->db);
  persist::createOrClear(pimpl->db, surface);
  transaction.commit();
}
Sint64
PixDocument::getLatestVersion()
{ return persist::getLatestVersion(pimpl->db); }

Sint64
PixDocument::newVersion(Sint64 currentVersion)
{ return persist::newVersion(pimpl->db, "Change", currentVersion); }

Sint64
PixDocument::putSurface(Sint64 pos, const Surface& surface)
{ return persist::putSurface(pimpl->db, pos, surface); }

void
PixDocument::getSurface(Sint64 version, Sint64 pos, Surface* surface)
{ persist::getSurface(pimpl->db, version, pos, surface); }

PixDocument::PixDocument(std::unique_ptr<impl> impl)
  : pimpl{std::move(impl)}
{
}

} // pixedit
