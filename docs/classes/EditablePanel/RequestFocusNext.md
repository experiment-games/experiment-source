---
template: lua-class-function.html
title: RequestFocusNext
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: EditablePanel
  function: RequestFocusNext
  realm: client
  description: "Requests focus for the next panel"
  
  arguments:
  - position: 1
    name: "editablePanel"
    type: EditablePanel
  - position: 2
    name: "panel"
    type: Panel
  returns:
    - type: boolean
      description: "True if the next panel received focus"
---

<div class="lua__search__keywords">
EditablePanel:RequestFocusNext &#x2013; Requests focus for the next panel
</div>
