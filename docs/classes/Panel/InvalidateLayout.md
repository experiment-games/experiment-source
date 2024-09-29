---
template: lua-class-function.html
title: InvalidateLayout
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: InvalidateLayout
  realm: client
  description: "Invalidates the layout of the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "shouldLayoutNow"
    type: boolean
    default: "false"
  - position: 3
    name: "shouldReloadScheme"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Panel:InvalidateLayout &#x2013; Invalidates the layout of the panel
</div>
