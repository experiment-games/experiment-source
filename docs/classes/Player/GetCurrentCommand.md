---
template: lua-class-function.html
title: GetCurrentCommand
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Player
  function: GetCurrentCommand
  realm: shared
  description: "Returns the last command which was sent by the current prediction player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  returns:
    - type: UserCommand
      description: "The last user command."
---

<div class="lua__search__keywords">
Player:GetCurrentCommand &#x2013; Returns the last command which was sent by the current prediction player.
</div>
