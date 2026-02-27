//
// Created by talesm on 24/02/2026.
//

#ifndef PIXEDITOR_PIXFORMAT_HPP
#define PIXEDITOR_PIXFORMAT_HPP

#include <nlohmann/json.hpp>

#include "Surface.hpp"

namespace pixedit {
using json = nlohmann::json;

class PixDocument
{
public:
  PixDocument();

  PixDocument(PixDocument&&) noexcept;

  PixDocument& operator=(PixDocument&&) noexcept;

  ~PixDocument();

  static PixDocument create(const std::string& filename,
                            const SDL::Point& size);

  static PixDocument convert(const std::string& filename,
                             const SDL::Surface& surface);

  static PixDocument load(const std::string& path);

  void clearContents(const SDL::Point& size = {});

  void clearContents(const SDL::Surface& surface);

  Sint64 getLatestVersion();

  Sint64 newVersion(Sint64 currentVersion);

  Sint64 putSurface(Sint64 pos, const Surface& surface);

  void getSurface(Sint64 version, Sint64 pos, Surface* surface);

private:
  struct impl;

  std::unique_ptr<impl> pimpl;

  PixDocument(std::unique_ptr<impl> impl);
};

} // pixedit

#endif // PIXEDITOR_PIXFORMAT_HPP
