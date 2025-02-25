---
template: lua-class-function.html
title: GetControlInt
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: EditablePanel
  function: GetControlInt
  realm: client
  description: "Gets an integer value from a control"
  
  arguments:
  - position: 1
    name: "editablePanel"
    type: EditablePanel
  - position: 2
    name: "control"
    type: string
  - position: 3
    name: "default"
    type: number
  returns:
    - type: integer
      description: "The control integer value"
---

<div class="lua__search__keywords">
EditablePanel:GetControlInt &#x2013; Gets an integer value from a control
</div>
