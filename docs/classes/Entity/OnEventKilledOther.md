---
template: lua-class-function.html
title: OnEventKilledOther
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: OnEventKilledOther
  realm: server
  description: "Called when the 'KilledOther' event happened."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "victim"
    type: Entity
  - position: 3
    name: "damageInfo"
    type: DamageInfo
  returns:
    
---

<div class="lua__search__keywords">
Entity:OnEventKilledOther &#x2013; Called when the 'KilledOther' event happened.
</div>
