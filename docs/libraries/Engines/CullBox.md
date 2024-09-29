---
template: lua-library-function.html
title: CullBox
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: CullBox
  realm: client
  description: "Cull a box."
  
  arguments:
  - position: 1
    name: "boxMin"
    type: Vector
  - position: 2
    name: "boxMax"
    type: Vector
  returns:
    - type: boolean
      description: "True if the box was culled, false otherwise."
---

<div class="lua__search__keywords">
Engines.CullBox &#x2013; Cull a box.
</div>
