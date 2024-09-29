---
template: lua-class-function.html
title: SetDuckSpeed
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: SetDuckSpeed
  realm: shared
  description: "Set the player's duck speed. This **should not** suffer from this bug that Garry's Mod has: https://github.com/Facepunch/garrysmod-issues/issues/2722"
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "speedInMilliseconds"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Player:SetDuckSpeed &#x2013; Set the player's duck speed. This **should not** suffer from this bug that Garry's Mod has: https://github.com/Facepunch/garrysmod-issues/issues/2722
</div>
