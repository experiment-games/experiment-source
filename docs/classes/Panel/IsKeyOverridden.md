---
template: lua-class-function.html
title: IsKeyOverridden
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: IsKeyOverridden
  realm: client
  description: "Checks if the key is overridden"
  
  arguments:
    - name: "keyCode"
      type: enumeration/BUTTON
    - name: "modifiers"
      type: number
  returns:
    - type: boolean
      description: "True if the key is overridden, false otherwise"
---

<div class="lua__search__keywords">
Panel:IsKeyOverridden &#x2013; Checks if the key is overridden
</div>
