---
template: lua-class-function.html
title: IsKeyRebound
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: IsKeyRebound
  realm: client
  description: "Checks if the key is rebound"
  
  arguments:
    - name: "keyCode"
      type: enumeration/BUTTON
    - name: "modifiers"
      type: number
  returns:
    - type: boolean
      description: "True if the key is rebound, false otherwise"
---

<div class="lua__search__keywords">
Panel:IsKeyRebound &#x2013; Checks if the key is rebound
</div>
