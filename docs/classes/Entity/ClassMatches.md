---
template: lua-class-function.html
title: ClassMatches
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: ClassMatches
  realm: server
  description: "Class matches."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "classNameOrWildcard"
    type: string
  returns:
    - type: boolean
      description: "true if matches, false otherwise."
---

<div class="lua__search__keywords">
Entity:ClassMatches &#x2013; Class matches.
</div>
