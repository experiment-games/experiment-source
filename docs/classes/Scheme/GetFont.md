---
template: lua-class-function.html
title: GetFont
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Scheme
  function: GetFont
  realm: client
  description: "Returns the font for the specified font name."
  
  arguments:
  - position: 1
    name: "scheme"
    type: Scheme
  - position: 2
    name: "fontName"
    type: string
  - position: 3
    name: "proportional"
    type: boolean
    default: "false"
  returns:
    - type: font
      description: "The font for the specified font name."
---

<div class="lua__search__keywords">
Scheme:GetFont &#x2013; Returns the font for the specified font name.
</div>
