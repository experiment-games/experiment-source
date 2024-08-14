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
  
  argumentSets:
    - arguments:
      - name: "key"
        type: string
      - name: "value"
        type: number
    - arguments:
      - name: "value"
        type: string
    - arguments:
      - name: "value"
        type: Vector
  returns:
    - type: boolean
      description: "True if key value, false otherwise."
---

<div class="lua__search__keywords">
Entity:KeyValue &#x2013; Key value.
</div>
