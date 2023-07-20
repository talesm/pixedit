Roadmap
=======

Here we have the our building strategy.

Version Goals
-------------

- [x] **v0.1 Table stakes**: Open/Save PNG and BMP, 24/32 bits, multiple images. Color selection and pseudo palette. Basic tools, copy and paste;
- [ ] **v0.2 Magic sauce**: Layers and animation, import/export, extension hooks for loader/save, effects, etc;
- [ ] **v0.3 Glyphs**: Tiling and text support;

To do
-----

- [x] Design format *.pix
- [ ] Implement *.pix for single image
- [ ] Layers
  - [ ] Implement Layer window
  - [ ] Implement *.pix for layered image
- [ ] Animation
  - [ ] Implement Animation timeline
  - [ ] Implement Animation options
  - [ ] Implement *.pix for animated
- [ ] Tools
  - [ ] Text* (using existing ImGui engine)
  - [ ] Color selection*
  - [ ] Contiguous color selection*
- [ ] Options
  - [ ] Color palette window (not palette image, just reference)*
- [ ] Create glass surface and better looking scale hooks

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
