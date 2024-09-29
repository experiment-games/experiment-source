---
template: lua-library-function.html
title: RandomFloatExponential
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Randoms
  function: RandomFloatExponential
  realm: shared
  description: "Generate a random float with an exponential distribution."
  
  arguments:
  - position: 1
    name: "min"
    type: number
    default: "0.0f"
  - position: 2
    name: "max"
    type: number
    default: "0.0f"
  - position: 3
    name: "exponent"
    type: number
    default: "1.0f"
  returns:
    - type: number
      description: "The generated random float."
---

<div class="lua__search__keywords">
Randoms.RandomFloatExponential &#x2013; Generate a random float with an exponential distribution.
</div>
