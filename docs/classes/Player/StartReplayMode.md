---
template: lua-class-function.html
title: StartReplayMode
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Player
  function: StartReplayMode
  realm: server
  description: "Start the replay mode for the player."
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "delay"
    type: number
  - position: 3
    name: "duration"
    type: number
  - position: 4
    name: "entityIndex"
    type: number
  returns:
    - type: boolean
      description: "Whether the replay mode was started."
---

<div class="lua__search__keywords">
Player:StartReplayMode &#x2013; Start the replay mode for the player.
</div>
