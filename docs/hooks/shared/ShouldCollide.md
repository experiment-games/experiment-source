---
template: lua-hook.html
title: ShouldCollide
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  function: ShouldCollide
  realm: shared
  description: "Called when two entities are about to collide. Return false to prevent the collision."
  arguments:
    - name: "entity1"
      description: "The entity colliding"
      type: Entity
    - name: "entity2"
      description: "The entity colliding with entity1"
      type: Entity
  returns:
    - type: unknown
      description: ""
---

<div class="lua__search__keywords">
hook ShouldCollide &#x2013; "Called when two entities are about to collide. Return false to prevent the collision."
</div>
