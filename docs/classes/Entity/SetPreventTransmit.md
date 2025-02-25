---
template: lua-class-function.html
title: SetPreventTransmit
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetPreventTransmit
  realm: server
  description: "Set prevent transmit."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    types:
      - name: "filter"
        type: RecipientFilter
      - name: "filter"
        type: table
      - name: "filter"
        type: Entity
  - position: 3
    name: "shouldPreventTransmitting"
    type: boolean
  returns:
    
---

<div class="lua__search__keywords">
Entity:SetPreventTransmit &#x2013; Set prevent transmit.
</div>
