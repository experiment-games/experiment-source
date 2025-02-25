---
template: lua-library-function.html
title: GetTextSize
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: GetTextSize
  realm: client
  description: "Gets the text size"
  
  arguments:
  - position: 1
    name: "font"
    type: FontHandle
  - position: 2
    name: "text"
    type: string
  returns:
    - type: integer
      description: "The width"
    - type: integer
      description: "The height"
---

<div class="lua__search__keywords">
Surfaces.GetTextSize &#x2013; Gets the text size
</div>
