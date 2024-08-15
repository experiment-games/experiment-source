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
  
  argumentSets:
    - arguments:
      - name: "angle"
        type: Angle
    - arguments:
      - name: "angleString"
        type: string
    - arguments:
      - name: "p"
        type: number
        default: "0.0f"
      - name: "y"
        type: number
        default: "0.0f"
      - name: "r"
        type: number
        default: "0.0f"
  returns:
    - type: Angle
      description: "The created angle."
---

<div class="lua__search__keywords">
Angles.Create &#x2013; Creates a new angle.
</div>
