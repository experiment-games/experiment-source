---
template: lua-class-function.html
title: TransferPoseParameters
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: TransferPoseParameters
  realm: server
  description: "Transfer pose parameters from one entity to another"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "target"
    type: Entity
  returns:
    - type: boolean
      description: "true if transfered, false if one of the entities wasn't valid."
---

<div class="lua__search__keywords">
Entity:TransferPoseParameters &#x2013; Transfer pose parameters from one entity to another
</div>
