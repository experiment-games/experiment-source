---
template: lua-class-function.html
title: __eq
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: EditablePanel
  function: __eq
  realm: client
  description: "Metamethod that is called when two panels are compared"
  
  arguments:
  - position: 1
    name: "editablePanel"
    type: EditablePanel
  - position: 2
    name: "other"
    type: EditablePanel
  returns:
    - type: boolean
      description: "Whether the panels are equal"
---

<div class="lua__search__keywords">
EditablePanel:__eq &#x2013; Metamethod that is called when two panels are compared
</div>
