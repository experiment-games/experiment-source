---
template: lua-hook.html
title: EntityNetworkVariableChanging
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  function: EntityNetworkVariableChanging
  realm: shared
  description: "Called just before a network variable is changed"
  arguments:
    - name: "entity"
      description: ""
      type: Entity
    - name: "slot"
      description: "which network variable"
      type: number
    - name: "newValue"
      description: ""
      type: any
    - name: "oldValue"
      description: ""
      type: any
  returns:
    
---

<div class="lua__search__keywords">
hook EntityNetworkVariableChanging &#x2013; "Called just before a network variable is changed"
</div>
