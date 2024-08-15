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
    - name: "parent"
      type: Panel
      default: "VGui_GetClientLuaRootPanel()"
    - name: "name"
      type: string
      default: "\"Html\""
    - name: "shouldAllowJavaScript"
      type: boolean
      default: "false"
    - name: "popupWindow"
      type: boolean
      default: "false"
  returns:
    - type: Html
      description: "The new HTML Panel"
---

<div class="lua__search__keywords">
Panels.Html &#x2013; Creates a new HTML panel
</div>
