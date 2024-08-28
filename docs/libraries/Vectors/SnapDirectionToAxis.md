---
template: lua-library-function.html
title: SnapDirectionToAxis
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Vectors
  function: SnapDirectionToAxis
  realm: server
  description: "Modifies a vector so it snaps to the nearest axis vector (if within epsilon)."
  
  arguments:
    - name: "direction"
      type: Vector
    - name: "epsilon"
      type: number
      default: "0.002f"
  returns:
    
---

<div class="lua__search__keywords">
Vectors.SnapDirectionToAxis &#x2013; Modifies a vector so it snaps to the nearest axis vector (if within epsilon).
</div>
