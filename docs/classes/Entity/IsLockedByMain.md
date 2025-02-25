---
template: lua-class-function.html
title: IsLockedByMain
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: IsLockedByMain
  realm: server
  description: "Whether this door is locked by the main door (described as 'master' door in keyvalues)."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: boolean
      description: "true if locked by main, false otherwise."
---

<div class="lua__search__keywords">
Entity:IsLockedByMain &#x2013; Whether this door is locked by the main door (described as 'master' door in keyvalues).
</div>
