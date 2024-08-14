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
  
  argumentSets:
    - arguments:
      - name: "filter"
        type: RecipientFilter
    - arguments:
      - name: "filter"
        type: table
    - arguments:
      - name: "filter"
        type: Entity
      - name: "shouldPreventTransmitting"
        type: boolean
  returns:
    
---

<div class="lua__search__keywords">
Entity:SetPreventTransmit &#x2013; Set prevent transmit.
</div>
