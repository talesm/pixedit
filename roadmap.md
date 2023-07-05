Roadmap
=======

Here we have the our building strategy.

Version Goals
-------------

- [ ] **v0.1 Table stakes**: Open/Save PNG and BMP, 24/32 bits, multiple images. Color selection and pseudo pallete. Basic tools, copy and paste;
- [ ] **v0.2 Magic sauce**: Layers and animation, import/export, extension hooks for loader/save, effects, etc;
- [ ] **v0.3 Glyphs**: Tiling and text support;

To do
-----

- [x] View image
  - [x] Load from drop
  - [x] Load C+o
  - [x] Save C+S | C+s
  - [x] Close C+w
  - [x] ~~Muliple files~~
  - [x] Zoom (C+wheel)
  - [x] Pan (C+click)
- [ ] History
  - [x] Undo
  - [x] Redo
  - [ ] Cancel action
- [x] Clipboard
  - [x] Copy
  - [ ] Copy selection
  - [ ] Paste
  - [ ] Paste as new
  - [ ] Internal* (as fallback??)
  - [x] X based support
- [ ] Tools
  - [x] Zoom
  - [x] Pan
  - [x] Free hand
  - [ ] Lines
  - [ ] Rectangle
  - [ ] Elipsis*
  - [ ] Polygon*
  - [ ] Pick color
  - [ ] Text* (using existing ImGui engine)
  - [ ] Selection (Alt remove, shift adds)
    - [ ] Rect
    - [ ] Free form
    - [ ] Color
    - [ ] Contiguous color
- [ ] New file dialog
- [ ] Exit confirmation dialog
- [ ] Options
  - [ ] ~~Color pallete (not pallete image, just reference)~~
  - [x] Color selector/Wheel
  - [x] Swap color
  - [ ] Mask
  - [ ] Tile
- [ ] File menu
  - [ ] New
  - [ ] Load 
  - [ ] Save
  - [ ] Save as 
  - [ ] Close
  - [ ] Exit
  - [ ] ~~Last files~~
- [ ] Edit menu
  - [ ] Copy
  - [ ] Paste
  - [ ] Paste as new
  - [ ] Undo/redo
- [ ] Status bar
  - [ ] Tool name
  - [ ] Mouse pos

Wishlist
--------

- Layers
- Better integrated open/save dialogs
- Animation
- Custom file support
- Reference palletes
- 8bit pallete mode
- 8bit grayscale/color scale mode
- Stamp (Colored tiles)
- File history
- Proper configuration support
- Rich file dialogs
- File browser
- Text support
