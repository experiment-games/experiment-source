---
template: lua-class-function.html
title: GetBodyGroupsAsString
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetBodyGroupsAsString
  realm: shared
  description: "Get the bodygroup values as a string of hexadecimal values. Each hexadecimal character represents the bodygroup at its index, e.g: 0a00001 means bodygroup 1 is 10(a) and bodygroup 6 is 1, the rest are 0"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: string
      description: "The bodygroup values as a string of hexadecimal values"
---

<div class="lua__search__keywords">
Entity:GetBodyGroupsAsString &#x2013; Get the bodygroup values as a string of hexadecimal values. Each hexadecimal character represents the bodygroup at its index, e.g: 0a00001 means bodygroup 1 is 10(a) and bodygroup 6 is 1, the rest are 0
</div>
