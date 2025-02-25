---
template: lua-class-function.html
title: GetRagdollOwner
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetRagdollOwner
  realm: shared
  description: "Returns the player this ragdoll came from. NULL if this is not a player ragdoll (will warn)"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: Player
      description: "The player this ragdoll came from"
---

<div class="lua__search__keywords">
Entity:GetRagdollOwner &#x2013; Returns the player this ragdoll came from. NULL if this is not a player ragdoll (will warn)
</div>
