---
template: lua-library-function.html
title: Create
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Angles
  function: Create
  realm: shared
  description: "Creates a new angle."
  
  arguments:
  - position: 1
    types:
      - name: "angle"
        type: Angle
      - name: "angleString"
        type: string
      - name: "p"
        type: number
    default: "0.0f"
  - position: 2
    name: "y"
    type: number
    default: "0.0f"
  - position: 3
    name: "r"
    type: number
    default: "0.0f"
  returns:
    - type: Angle
      description: "The created angle."
---

<div class="lua__search__keywords">
Angles.Create &#x2013; Creates a new angle.
</div>
