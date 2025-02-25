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
  - position: 1
    name: "origin"
    type: Vector
  - position: 2
    name: "angle"
    type: Angle
  - position: 3
    name: "radius"
    type: number
  - position: 4
    name: "shouldPunchPlayersInAir"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Players.ViewPunchAll &#x2013; Punches the view of all clients within radius. If radius is 0, punches all clients..
</div>
