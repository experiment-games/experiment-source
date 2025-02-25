---
template: lua-hook.html
title: GetPlayerTextColor
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  function: GetPlayerTextColor
  realm: client
  description: "Allows overriding the color of the player's name in the scoreboard."
  arguments:
    - name: "player"
      description: ""
      type: Entity
    - name: "color"
      description: "the color of the player's name in the scoreboard"
      type: Color
  returns:
    - type: color
      description: "return a replacement color to override the default color"
---

<div class="lua__search__keywords">
hook GetPlayerTextColor &#x2013; "Allows overriding the color of the player's name in the scoreboard."
</div>
