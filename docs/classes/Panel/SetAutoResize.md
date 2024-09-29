---
template: lua-class-function.html
title: SetAutoResize
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: SetAutoResize
  realm: client
  description: "Sets the auto resize"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "pinCorner"
    type: enumeration/PIN_CORNER
  - position: 3
    name: "autoResize"
    type: enumeration/AUTO_RESIZE
  - position: 4
    name: "pinOffsetX"
    type: number
  - position: 5
    name: "pinOffsetY"
    type: number
  - position: 6
    name: "unpinnedCornerOffsetX"
    type: number
  - position: 7
    name: "unpinnedCornerOffsetY"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Panel:SetAutoResize &#x2013; Sets the auto resize
</div>
