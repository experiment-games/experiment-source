---
template: lua-library-function.html
title: SharedRandomInt
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Randoms
  function: SharedRandomInt
  realm: shared
  description: "Generate a shared random integer. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number"
  
  arguments:
    - name: "sharedName"
      type: string
    - name: "min"
      type: number
    - name: "max"
      type: number
    - name: "additionalSeed"
      type: number
      default: "0"
  returns:
    - type: integer
      description: "The generated random integer."
---

<div class="lua__search__keywords">
Randoms.SharedRandomInt &#x2013; Generate a shared random integer. Whenever generating random numbers in shared/predicted code, these SharedRandom* functions have to be used. Each call should specify a unique 'sharedName' that seeds the random number generator. In loops make sure the additionalSeed is increased with the loop counter, otherwise it will always return the same random number
</div>
