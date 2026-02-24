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

void
PixDocument::clearContents(const SDL::Point& size)
{
  SQLite::Transaction transaction(pimpl->db);
  persist::createOrClear(pimpl->db);
  transaction.commit();
}

void
PixDocument::clearContents(const SDL::Surface& surface)
{
  SQLite::Transaction transaction(pimpl->db);
  persist::createOrClear(pimpl->db, surface);
  transaction.commit();
}

PixDocument::PixDocument(std::unique_ptr<impl> impl)
  : pimpl{std::move(impl)}
{
}

} // pixedit
