---
template: lua-library-function.html
title: Frame
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: Frame
  realm: client
  description: "Creates a new Frame panel"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "name"
    type: string
    default: "\"Frame\""
  returns:
    - type: Frame
      description: "The new Frame Panel"
---

<div class="lua__search__keywords">
Panels.Frame &#x2013; Creates a new Frame panel
</div>
