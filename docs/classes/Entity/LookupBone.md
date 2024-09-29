---
template: lua-class-function.html
title: LookupBone
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: LookupBone
  realm: shared
  description: "Lookup the bone."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "boneName"
    type: string
  returns:
    - type: integer
      description: "The bone"
---

<div class="lua__search__keywords">
Entity:LookupBone &#x2013; Lookup the bone.
</div>
