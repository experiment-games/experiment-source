---
template: lua-class-function.html
title: RetrieveRagdollInfo
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: RetrieveRagdollInfo
  realm: client
  description: "Retrieve the ragdoll info."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: boolean
      description: "Whether the operation was successful"
    - type: Vector
      description: "The position"
---

<div class="lua__search__keywords">
Entity:RetrieveRagdollInfo &#x2013; Retrieve the ragdoll info.
</div>
