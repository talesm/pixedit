Roadmap
=======

Here we have the our building strategy.

Version Goals
-------------

- [ ] **v0.1 Table stakes**: Open/Save PNG and BMP, 24/32 bits, multiple images. Color selection and pseudo palette. Basic tools, copy and paste;
- [ ] **v0.2 Magic sauce**: Layers and animation, import/export, extension hooks for loader/save, effects, etc;
- [ ] **v0.3 Glyphs**: Tiling and text support;

To do
-----

- [x] View image
  - [x] Load from drop
  - [x] Load C+o
  - [x] Save C+S | C+s
  - [x] Close C+w
  - [x] Multiple files
  - [x] Zoom (C+wheel)
  - [x] Pan (C+click)
- [x] History
  - [x] Undo
  - [x] Redo
  - [x] Cancel action
- [x] Clipboard
  - [x] Copy
  - [x] Copy selection
  - [x] Paste
  - [x] Paste as new
  - [x] Internal* (as fallback??)
  - [x] X based support
  - [x] Paste image on current offset
- [ ] Tools
  - [x] Zoom
  - [x] Move
  - [x] Free hand
  - [x] Lines
  - [x] Rectangle
  - [x] Polygon
  - [x] Ellipsis*
  - [x] Flood fill
  - [x] Pick color
  - [ ] Text* (using existing ImGui engine)
  - [ ] Selection
    - [x] Rect
      - [x] Make selection persist over cycling buffer focus
    - [ ] Free form
    - [ ] Color
    - [ ] Contiguous color
- [x] "New file" dialog
- [ ] "Exit confirmation" dialog
- [ ] Options
  - [ ] Color palette (not palette image, just reference)*
  - [x] Color selector/Wheel
  - [x] Swap color
  - [ ] Pen
  - [ ] Tile
- [ ] ~~Tabs for switching files~~
- [ ] Menu bar
  - [x] File menu
    - [x] New
    - [x] Load 
    - [x] Save
    - [x] Save as 
    - [x] Close
    - [x] Exit
    - [ ] ~~Last files~~
  - [x] Edit menu
    - [x] Copy
    - [x] Paste
    - [x] Paste as new
    - [x] Undo/redo
  - [ ] Help/About
- [ ] Status bar*
  - [ ] Tool name
  - [ ] Mouse pos
- [x] Windowed mode*
- [ ] Update title with filename, dirty "*"

Wishlist
--------

- Layers
- Better integrated open/save dialogs
- Animation
- Custom file support
- Reference palettes
- 8bit palette mode
- 8bit gray-scale/color scale mode
- Stamp (Colored tiles)
- File history
- Proper configuration support
- Rich file dialogs
- File browser
- Add / Remove to selection
- Use selection as stencil
- Text support
