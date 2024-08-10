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
    - name: "entindex "
      type: number
    - name: "color[0] "
      type: number
    - name: "color[1] "
      type: number
    - name: "color[2] "
      type: number
  returns:
    - type: number
      description: "red component of the color"
    - type: number
      description: "green component"
    - type: number
      description: "blue component"
---

<div class="lua__search__keywords">
hook GetPlayerTextColor &#x2013; "Allows overriding the color of the player's name in the scoreboard."
</div>
