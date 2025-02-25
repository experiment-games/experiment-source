---
template: lua-library-function.html
title: GetPlayerConnectionInfo
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Players
  function: GetPlayerConnectionInfo
  realm: shared
  description: "Get the connection info of a player."
  
  arguments:
  - position: 1
    name: "index"
    type: number
  returns:
    - type: integer
      description: "The player's ping."
    - type: integer
      description: "The player's packet loss."
---

<div class="lua__search__keywords">
Players.GetPlayerConnectionInfo &#x2013; Get the connection info of a player.
</div>
