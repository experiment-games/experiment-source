---
template: lua-class-function.html
title: KeyValue
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: KeyValue
  realm: shared
  description: "Key value."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "key"
    type: string
  - position: 3
    types:
      - name: "value"
        type: number
      - name: "value"
        type: string
      - name: "value"
        type: Vector
  returns:
    - type: boolean
      description: "True if key value, false otherwise."
---

<div class="lua__search__keywords">
Entity:KeyValue &#x2013; Key value.
</div>
