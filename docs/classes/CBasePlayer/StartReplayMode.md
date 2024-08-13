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
  library: CBasePlayer
  function: StartReplayMode
  realm: server
  description: "Start the replay mode for the player."
  
  arguments:
    - name: "delay"
      type: number
    - name: "duration"
      type: number
    - name: "entityIndex"
      type: integer
  returns:
    - type: boolean
      description: "Whether the replay mode was started."
---

<div class="lua__search__keywords">
CBasePlayer:StartReplayMode &#x2013; Start the replay mode for the player.
</div>
