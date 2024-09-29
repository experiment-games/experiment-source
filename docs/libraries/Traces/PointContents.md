---
template: lua-library-function.html
title: PointContents
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Traces
  function: PointContents
  realm: shared
  description: "Returns the contents mask + entity at a particular world-space position."
  
  arguments:
  - position: 1
    name: "point"
    type: Vector
  returns:
    - type: integer
      description: "Bitwise contents mask."
---

<div class="lua__search__keywords">
Traces.PointContents &#x2013; Returns the contents mask + entity at a particular world-space position.
</div>
