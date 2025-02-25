---
template: lua-class-function.html
title: DoesHavePlayerChild
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: DoesHavePlayerChild
  realm: server
  description: "Whether this entity has a player child somewhere in the hierarchy (checks EFL_HAS_PLAYER_CHILD)."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: boolean
      description: "true if has player child, false otherwise."
---

<div class="lua__search__keywords">
Entity:DoesHavePlayerChild &#x2013; Whether this entity has a player child somewhere in the hierarchy (checks EFL_HAS_PLAYER_CHILD).
</div>
