---
template: lua-class-function.html
title: GetFrags
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetFrags
  realm: shared
  description: "Get the player's frags (+1 for a kill, -1 for suicide)."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  returns:
    - type: integer
      description: "The player's frags."
---

<div class="lua__search__keywords">
Player:GetFrags &#x2013; Get the player's frags (+1 for a kill, -1 for suicide).
</div>
