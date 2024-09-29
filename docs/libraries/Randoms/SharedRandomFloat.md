---
template: lua-library-function.html
title: SharedRandomFloat
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Randoms
  function: SharedRandomFloat
  realm: shared
  description: "Generate a shared random float. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number"
  
  arguments:
  - position: 1
    name: "sharedName"
    type: string
  - position: 2
    name: "min"
    type: number
  - position: 3
    name: "max"
    type: number
  - position: 4
    name: "additionalSeed"
    type: number
    default: "0"
  returns:
    - type: number
      description: "The generated random float."
---

<div class="lua__search__keywords">
Randoms.SharedRandomFloat &#x2013; Generate a shared random float. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number
</div>
