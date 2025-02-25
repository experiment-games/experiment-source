---
template: lua-library-function.html
title: SetFontGlyphSet
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: SetFontGlyphSet
  realm: client
  description: "Sets the font glyph set"
  
  arguments:
  - position: 1
    name: "font"
    type: FontHandle
  - position: 2
    name: "fontName"
    type: string
  - position: 3
    name: "tall"
    type: number
  - position: 4
    name: "weight"
    type: number
  - position: 5
    name: "blur"
    type: number
  - position: 6
    name: "scanlines"
    type: number
  - position: 7
    name: "flags"
    type: number
  - position: 8
    name: "rangeMin"
    type: number
    default: "0"
  - position: 9
    name: "rangeMax"
    type: number
    default: "0"
  returns:
    - type: boolean
      description: "True if the font glyph set was set, false otherwise."
---

<div class="lua__search__keywords">
Surfaces.SetFontGlyphSet &#x2013; Sets the font glyph set
</div>
