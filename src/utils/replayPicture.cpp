#include "replayPicture.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "PictureView.hpp"

namespace pixedit {

struct CommandEvaluator
{
  PictureView& view;
  bool allowLoad;
  bool needsFlushing = true;
  bool dataMode = false;

  void eval(std::istream& in)
  {
    std::string line;
    while (std::getline(in, line)) {
      if (line.empty() || line[0] == '#') continue;
      evalLine(line);
      if (dataMode) {
        dataMode = false;
        flush();
        evalData(in, view.getBuffer()->getSurface());
      }
    }
  }

  void evalData(std::istream& in, Surface s)
  {
    Uint32 pixel = 0;
    in >> std::hex;
    for (int y = 0; y < s.getH(); ++y) {
      for (int x = 0; x < s.getW(); ++x) {
        in >> pixel;
        s.setPixel(x, y, pixel);
      }
    }
    in >> std::dec;
  }

  void evalLine(std::string line)
  {
    std::stringstream sline{line};
    std::string cmd;
    sline >> cmd;
    if (cmd == "FORMAT") {
      int w = 1, h = 1;
      sline >> w >> h;
      setSurface(Surface::create(w, h));
    } else if (cmd == "DATA:") {
      dataMode = true;
    } else if (cmd == "LEFT") {
      sline >> cmd;
      view.state.left = cmd != "UP";
    } else if (cmd == "RIGHT") {
      sline >> cmd;
      view.state.right = cmd != "UP";
    } else if (cmd == "POS") {
      int x, y;
      sline >> x >> y;
      view.state.x = x;
      view.state.y = y;
    } else if (cmd == "POS+") {
      int x, y;
      sline >> x >> y;
      view.state.x += x;
      view.state.y += y;
    } else if (cmd == "LOAD" && allowLoad) {
      line.erase(0, 5);
      while (line.ends_with(' ')) { line.erase(line.size() - 1); }
      while (line.starts_with(' ')) { line.erase(0, 1); }
      load(line);
    } else if (cmd == "FLUSH") {
      flush();
    }
  }

  void setSurface(Surface surface)
  {
    view.setBuffer(std::make_shared<PictureBuffer>("", std::move(surface)));
    view.update(nullptr);
    needsFlushing = false;
  }

  bool load(const std::string& filename)
  {
    if (filename.ends_with(".txt")) {
      std::ifstream fin(filename);
      bool allowLoadBkp = allowLoad;
      allowLoad = false;
      eval(fin);
      allowLoad = allowLoadBkp;
    } else {
      PictureBuffer b{filename};
      if (!b.getSurface()) {
        std::cerr << SDL_GetError();
        return false;
      }
      setSurface(b.getSurface());
    }
    return true;
  }

  void flush()
  {
    if (!needsFlushing) { return; }
    if (!view.getBuffer()) {
      view.setBuffer(
        std::make_shared<PictureBuffer>("", Surface::create(1, 1)));
    }
    view.update(nullptr);
    needsFlushing = false;
  }
};

void
replayPicture(std::istream& in, PictureView& view, bool allowLoad)
{
  CommandEvaluator ctx{view, allowLoad};
  ctx.eval(in);
  ctx.flush();
}

Surface
replayPicture(std::istream& in, std::string_view filename, bool allowLoad)
{
  PictureView view{{0, 0, 1, 1}};
  CommandEvaluator ctx{view, allowLoad};
  if (!filename.empty() && !ctx.load(std::string(filename))) {
    std::cerr << "File '" << filename << "' could not be loaded\n";
  }
  ctx.eval(in);
  ctx.flush();
  auto b = view.getBuffer();
  return b ? b->getSurface() : nullptr;
}

} // namespace pixedit
