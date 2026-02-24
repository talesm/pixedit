//
// Created by talesm on 11/02/2026.
//
#include "PixDocument.hpp"
#include "Surface.hpp"

namespace pixedit {

void
savePixSurface(const Surface& surface, const std::string& filename)
{ PixDocument doc = PixDocument::convert(filename, surface); }

}
