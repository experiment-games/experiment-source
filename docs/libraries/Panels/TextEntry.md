---
template: lua-library-function.html
title: TextEntry
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: TextEntry
  realm: client
  description: "Creates a new TextEntry panel"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "name"
    type: string
    default: "\"TextEntry\""
  returns:
    - type: TextEntry
      description: "The new TextEntry Panel"
---

<div class="lua__search__keywords">
Panels.TextEntry &#x2013; Creates a new TextEntry panel
</div>
