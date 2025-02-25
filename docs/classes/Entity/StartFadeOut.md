---
template: lua-class-function.html
title: StartFadeOut
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: StartFadeOut
  realm: server
  description: "Slowly fades a entity out, then removes it."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "duration"
    type: number
    default: "10.0f"
  - position: 3
    name: "instant"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Entity:StartFadeOut &#x2013; Slowly fades a entity out, then removes it.
</div>
