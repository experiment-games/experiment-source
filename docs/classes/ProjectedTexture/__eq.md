---
template: lua-class-function.html
title: __eq
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: ProjectedTexture
  function: __eq
  realm: client
  description: "Metamethod that is called when comparing two entities"
  
  arguments:
  - position: 1
    name: "projectedTexture"
    type: ProjectedTexture
  - position: 2
    name: "other"
    type: ProjectedTexture
  returns:
    - type: boolean
      description: "True if entities are equal."
---

<div class="lua__search__keywords">
ProjectedTexture:__eq &#x2013; Metamethod that is called when comparing two entities
</div>
