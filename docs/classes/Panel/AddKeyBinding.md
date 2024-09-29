---
template: lua-class-function.html
title: AddKeyBinding
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: AddKeyBinding
  realm: client
  description: "Adds a key binding to the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "key"
    type: string
  - position: 3
    name: "keyCode"
    type: enumeration/BUTTON
  - position: 4
    name: "modifiers"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Panel:AddKeyBinding &#x2013; Adds a key binding to the panel
</div>
