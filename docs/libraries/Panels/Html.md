---
template: lua-library-function.html
title: Html
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: Html
  realm: client
  description: "Creates a new HTML panel"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "name"
    type: string
    default: "\"Html\""
  - position: 3
    name: "shouldAllowJavaScript"
    type: boolean
    default: "false"
  - position: 4
    name: "popupWindow"
    type: boolean
    default: "false"
  returns:
    - type: Html
      description: "The new HTML Panel"
---

<div class="lua__search__keywords">
Panels.Html &#x2013; Creates a new HTML panel
</div>
