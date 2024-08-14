---
template: lua-class-function.html
title: PhysicsSplash
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: PhysicsSplash
  realm: server
  description: "Physics splash."
  
  arguments:
    - name: "center"
      type: Vector
    - name: "normal"
      type: Vector
    - name: "rawSpeed"
      type: number
    - name: "scaledSpeed"
      type: number
  returns:
    - type: boolean
      description: "true if splashed, false otherwise."
---

<div class="lua__search__keywords">
Entity:PhysicsSplash &#x2013; Physics splash.
</div>
