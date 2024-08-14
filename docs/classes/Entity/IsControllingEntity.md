---
template: lua-class-function.html
title: IsControllingEntity
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: IsControllingEntity
  realm: server
  description: "Whether we are controlling the given entity (train, tank, etc.) "
  
  arguments:
    - name: "testEntityUnderControl"
      type: Entity
  returns:
    - type: boolean
      description: "true if on controls, false otherwise."
---

<div class="lua__search__keywords">
Entity:IsControllingEntity &#x2013; Whether we are controlling the given entity (train, tank, etc.) 
</div>
