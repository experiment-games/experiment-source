---
template: lua-library-function.html
title: ModelImagePanel
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: ModelImagePanel
  realm: client
  description: "Creates a new model image panel"
  
  arguments:
    - name: "parent"
      type: Panel
      default: "VGui_GetClientLuaRootPanel()"
    - name: "name"
      type: string
      default: "\"ModelImagePanel\""
  returns:
    - type: ModelImagePanel
      description: "The new model image panel"
---

<div class="lua__search__keywords">
Panels.ModelImagePanel &#x2013; Creates a new model image panel
</div>
