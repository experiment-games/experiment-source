---
template: lua-library-function.html
title: GetFontAscent
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: GetFontAscent
  realm: client
  description: "Gets the font ascent"
  
  arguments:
  - position: 1
    name: "font"
    type: FontHandle
  - position: 2
    name: "text"
    type: string
  returns:
    - type: integer
      description: "The font ascent"
---

<div class="lua__search__keywords">
Surfaces.GetFontAscent &#x2013; Gets the font ascent
</div>
