---
template: lua-class-function.html
title: GetSubMaterialOverride
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetSubMaterialOverride
  realm: shared
  description: "Get the sub material override by it's index (which starts at 0 and runs until at most 31)"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "subMaterialIndex"
    type: number
  returns:
    - type: string
      description: "The sub material override"
---

<div class="lua__search__keywords">
Entity:GetSubMaterialOverride &#x2013; Get the sub material override by it's index (which starts at 0 and runs until at most 31)
</div>
