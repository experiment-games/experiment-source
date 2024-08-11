---
template: lua-class-function.html
title: PhysicsInit
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: CBaseEntity
  function: PhysicsInit
  realm: shared
  description: "Initialize physics object."
  
  arguments:
    - name: "solidType"
      type: integer
      default: "SOLID_VPHYSICS"
    - name: "solidFlags"
      type: number
      default: "0"
    - name: "createAsleep"
      type: boolean
      default: "false"
  returns:
    
---

<div class="lua__search__keywords">
CBaseEntity:PhysicsInit &#x2013; Initialize physics object.
</div>
