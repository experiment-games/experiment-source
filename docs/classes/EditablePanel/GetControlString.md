---
template: lua-class-function.html
title: GetControlString
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: EditablePanel
  function: GetControlString
  realm: client
  description: "Gets a string value from a control"
  
  arguments:
  - position: 1
    name: "editablePanel"
    type: EditablePanel
  - position: 2
    name: "control"
    type: string
  - position: 3
    name: "default"
    type: string
    default: "\"\""
  returns:
    - type: string
      description: "The control string value"
---

<div class="lua__search__keywords">
EditablePanel:GetControlString &#x2013; Gets a string value from a control
</div>
