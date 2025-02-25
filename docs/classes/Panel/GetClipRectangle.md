---
template: lua-class-function.html
title: GetClipRectangle
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: GetClipRectangle
  realm: client
  description: "Gets the clipping rectangle of the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  returns:
    - type: integer
      description: "The x position of the clipping rectangle"
    - type: integer
      description: "The y position of the clipping rectangle"
    - type: integer
      description: "The width of the clipping rectangle"
    - type: integer
      description: "The height of the clipping rectangle"
---

<div class="lua__search__keywords">
Panel:GetClipRectangle &#x2013; Gets the clipping rectangle of the panel
</div>
