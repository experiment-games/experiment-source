---
template: lua-library-function.html
title: Angle
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: _G
  function: Angle
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
    - type: angle
      description: "The created angle."
---

<div class="lua__search__keywords">
_G.Angle &#x2013; Creates a new angle.
</div>
