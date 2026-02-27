//
// Created by talesm on 11/02/2026.
//

#ifndef PIXEDITOR_SAVESURFACE_HPP
#define PIXEDITOR_SAVESURFACE_HPP

#include "Surface.hpp"

namespace pixedit {

void
savePixSurface(const Surface& surface, const std::string& filename);

Surface
loadPixSurface(const std::string& filename);

}

#endif // PIXEDITOR_SAVESURFACE_HPP
