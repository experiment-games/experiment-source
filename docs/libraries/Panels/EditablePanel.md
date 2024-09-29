---
template: lua-library-function.html
title: EditablePanel
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: EditablePanel
  realm: client
  description: "Creates a new editable panel"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "name"
    type: string
    default: "\"EditablePanel\""
  returns:
    - type: EditablePanel
      description: "The new editable panel"
---

<div class="lua__search__keywords">
Panels.EditablePanel &#x2013; Creates a new editable panel
</div>
