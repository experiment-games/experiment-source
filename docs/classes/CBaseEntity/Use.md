---
template: lua-class-function.html
title: Use
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: CBaseEntity
  function: Use
  realm: server
  description: "Dispatches use events to this entity's use handler, set via SetUse."
  
  arguments:
    - name: "activator"
      type: Entity
    - name: "caller"
      type: Entity
    - name: "useType"
      type: integer
    - name: "value"
      type: number
  returns:
    
---

<div class="lua__search__keywords">
CBaseEntity:Use &#x2013; Dispatches use events to this entity's use handler, set via SetUse.
</div>
