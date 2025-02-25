---
template: lua-class-function.html
title: GetViewModel
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetViewModel
  realm: shared
  description: "Get the player's view model."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "viewModelIndex"
    type: number
    default: "0"
  returns:
    - type: Entity
      description: "The player's view model."
---

<div class="lua__search__keywords">
Player:GetViewModel &#x2013; Get the player's view model.
</div>
