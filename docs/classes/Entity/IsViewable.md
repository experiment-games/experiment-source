---
template: lua-class-function.html
title: IsViewable
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: IsViewable
  realm: server
  description: "Is this entity something that would be looked at (model, sprite, etc.)."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: boolean
      description: "true if viewable, false otherwise."
---

<div class="lua__search__keywords">
Entity:IsViewable &#x2013; Is this entity something that would be looked at (model, sprite, etc.).
</div>
