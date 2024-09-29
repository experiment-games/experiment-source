---
template: lua-class-function.html
title: FindTransitionSequence
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: FindTransitionSequence
  realm: shared
  description: "Find the transition sequence."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "sequence"
    type: number
  - position: 3
    name: "goalSequence"
    type: number
  returns:
    - type: integer
      description: "The transition sequence"
    - type: integer
      description: "The direction"
---

<div class="lua__search__keywords">
Entity:FindTransitionSequence &#x2013; Find the transition sequence.
</div>
