---
template: lua-class-function.html
title: RequestFocusPrev
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: EditablePanel
  function: RequestFocusPrev
  realm: client
  description: "Requests focus for the previous panel"
  
  arguments:
  - position: 1
    name: "editablePanel"
    type: EditablePanel
  - position: 2
    name: "panel"
    type: Panel
  returns:
    - type: boolean
      description: "True if the previous panel received focus"
---

<div class="lua__search__keywords">
EditablePanel:RequestFocusPrev &#x2013; Requests focus for the previous panel
</div>
