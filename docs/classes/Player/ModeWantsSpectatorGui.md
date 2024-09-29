---
template: lua-class-function.html
title: ModeWantsSpectatorGui
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: ModeWantsSpectatorGui
  realm: server
  description: "Check if the observer mode wants the spectator GUI for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "mode"
    type: enumeration/OBSERVER_MODE
  returns:
    - type: boolean
      description: "Whether the observer mode wants the spectator GUI."
---

<div class="lua__search__keywords">
Player:ModeWantsSpectatorGui &#x2013; Check if the observer mode wants the spectator GUI for the player.
</div>
