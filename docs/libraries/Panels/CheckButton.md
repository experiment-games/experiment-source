---
template: lua-library-function.html
title: CheckButton
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: CheckButton
  realm: client
  description: "Creates a new CheckButton panel"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "name"
    type: string
    default: "\"CheckButton\""
  - position: 3
    name: "text"
    type: string
    default: "\"\""
  returns:
    - type: CheckButton
      description: "The new CheckButton Panel"
---

<div class="lua__search__keywords">
Panels.CheckButton &#x2013; Creates a new CheckButton panel
</div>
