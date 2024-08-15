---
template: lua-library-function.html
title: FindByIndex
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: FindByIndex
  realm: server
  description: "Gets an entity by its index. Might be the same as FindByEdictNumber?"
  
  arguments:
    - name: "index"
      type: number
  returns:
    - type: Entity
      description: "The entity found, or NULL if not found."
---

<div class="lua__search__keywords">
Entities.FindByIndex &#x2013; Gets an entity by its index. Might be the same as FindByEdictNumber?
</div>
