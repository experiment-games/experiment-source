---
template: lua-library-function.html
title: ClientPrint
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Players
  function: ClientPrint
  realm: shared
  description: "Print message to client"
  
  arguments:
  - position: 1
    name: "player"
    type: Player
  - position: 2
    name: "msgDest"
    type: number
  - position: 3
    name: "message"
    type: string
  - position: 4
    name: "param1"
    type: string
    default: "0"
  - position: 5
    name: "param2"
    type: string
    default: "0"
  - position: 6
    name: "param3"
    type: string
    default: "0"
  - position: 7
    name: "param4"
    type: string
    default: "0"
  returns:
    
---

<div class="lua__search__keywords">
Players.ClientPrint &#x2013; Print message to client
</div>
