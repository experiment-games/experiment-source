---
template: lua-class-function.html
title: GetBounds
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: GetBounds
  realm: client
  description: "Gets the bounds of the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  returns:
    - type: integer
      description: "The x position of the panel"
    - type: integer
      description: "The y position of the panel"
    - type: integer
      description: "The width of the panel"
    - type: integer
      description: "The height of the panel"
---

<div class="lua__search__keywords">
Panel:GetBounds &#x2013; Gets the bounds of the panel
</div>
