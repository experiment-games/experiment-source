---
template: lua-class-function.html
title: SetAvoidPlayers
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: SetAvoidPlayers
  realm: shared
  description: "Set whether to push away players in the same team. The pushing away is handled client-side."
  
  arguments:
  - position: 1
    name: "entity"
    type: Player
  - position: 2
    name: "shouldAvoidPlayers"
    type: boolean
  returns:
    
---

<div class="lua__search__keywords">
Player:SetAvoidPlayers &#x2013; Set whether to push away players in the same team. The pushing away is handled client-side.
</div>
