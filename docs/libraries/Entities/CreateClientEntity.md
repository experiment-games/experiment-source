---
template: lua-library-function.html
title: CreateClientEntity
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: CreateClientEntity
  realm: client
  description: "Create a clientside entity with the specified model."
  
  arguments:
    - name: "model"
      type: string
    - name: "renderGroup"
      type: number
      default: "RENDER_GROUP_OTHER"
  returns:
    - type: Entity
      description: "Clientside model entity."
---

<div class="lua__search__keywords">
Entities.CreateClientEntity &#x2013; Create a clientside entity with the specified model.
</div>
