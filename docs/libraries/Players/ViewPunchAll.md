---
template: lua-library-function.html
title: ViewPunchAll
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Players
  function: ViewPunchAll
  realm: shared
  description: "Punches the view of all clients within radius. If radius is 0, punches all clients.."
  
  arguments:
    - name: "origin"
      type: Vector
    - name: "angle"
      type: Angle
    - name: "radius"
      type: number
    - name: "shouldPunchPlayersInAir"
      type: boolean
      default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Players.ViewPunchAll &#x2013; Punches the view of all clients within radius. If radius is 0, punches all clients..
</div>
