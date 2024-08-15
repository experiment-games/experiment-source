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
  library: Entity
  function: Use
  realm: server
  description: "Dispatches use events to this entity's use handler, set via SetUse."
  
  arguments:
    - name: "activator"
      type: Entity
    - name: "caller"
      type: Entity
    - name: "useType"
      type: enumeration/USE_TYPE
    - name: "value"
      type: number
  returns:
    
---

<div class="lua__search__keywords">
Entity:Use &#x2013; Dispatches use events to this entity's use handler, set via SetUse.
</div>
