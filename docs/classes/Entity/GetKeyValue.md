---
template: lua-class-function.html
title: GetKeyValue
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetKeyValue
  realm: shared
  description: "Get key value (the value is limited to 256 bytes)."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "key"
    type: string
  returns:
    - type: boolean
      description: "True if key value exists, false otherwise."
    - type: string
      description: "The key value."
---

<div class="lua__search__keywords">
Entity:GetKeyValue &#x2013; Get key value (the value is limited to 256 bytes).
</div>
