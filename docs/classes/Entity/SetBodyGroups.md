---
template: lua-class-function.html
title: SetBodyGroups
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetBodyGroups
  realm: shared
  description: "Set the bodygroup values by the bodygroup string. Each hexadecimal character represents the bodygroup at its index, e.g: 0a00001 sets bodygroup 1 to 10(a) and bodygroup 6 to 1, the rest are set to 0"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "bodyGroupString"
    type: string
  returns:
    
---

<div class="lua__search__keywords">
Entity:SetBodyGroups &#x2013; Set the bodygroup values by the bodygroup string. Each hexadecimal character represents the bodygroup at its index, e.g: 0a00001 sets bodygroup 1 to 10(a) and bodygroup 6 to 1, the rest are set to 0
</div>
