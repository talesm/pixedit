//
// Created by talesm on 11/02/2026.
//
#include "PixDocument.hpp"
#include "Surface.hpp"

namespace pixedit {

void
savePixSurface(const Surface& surface, const std::string& filename)
{ PixDocument doc = PixDocument::convert(filename, surface); }

Surface
loadPixSurface(const std::string& filename)
{
  try {
    PixDocument doc = PixDocument::load(filename);
    return doc.getSurface(doc.getLatestVersion(), 1);
  } catch (std::exception& e) {
    SDL::Log("Can not load because: {}", e.what());
    return nullptr;
  }
}

}
