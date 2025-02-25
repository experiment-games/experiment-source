---
template: lua-library-function.html
title: DoesBoxTouchAreaFrustum
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: DoesBoxTouchAreaFrustum
  realm: client
  description: "Check if a box touches an area frustum."
  
  arguments:
  - position: 1
    name: "boxMin"
    type: Vector
  - position: 2
    name: "boxMax"
    type: Vector
  - position: 3
    name: "area"
    type: number
  returns:
    - type: boolean
      description: "True if the box touches the area frustum, false otherwise."
---

<div class="lua__search__keywords">
Engines.DoesBoxTouchAreaFrustum &#x2013; Check if a box touches an area frustum.
</div>
