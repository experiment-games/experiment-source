---
template: lua-class-function.html
title: SetSubMaterialOverride
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetSubMaterialOverride
  realm: shared
  description: "Set the sub material override by it's index (which starts at 0 and runs until at most 31). If called without arguments, it will reset all submaterials."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "subMaterialIndex"
    type: number
    nillable: true
  - position: 3
    name: "materialName"
    type: string
    nillable: true
  returns:
    
---

<div class="lua__search__keywords">
Entity:SetSubMaterialOverride &#x2013; Set the sub material override by it's index (which starts at 0 and runs until at most 31). If called without arguments, it will reset all submaterials.
</div>
