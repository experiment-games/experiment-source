---
template: lua-library-function.html
title: ClientPrintToAll
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Players
  function: ClientPrintToAll
  realm: shared
  description: "Print text to all clients, optionally substituting 4 strings. The type decides where to show it: HUD_PRINTCENTER (1), HUD_PRINTNOTIFY (2), HUD_PRINTTALK (3), HUD_PRINTCONSOLE (4) - Enums don't exist yet"
  
  arguments:
  - position: 1
    name: "messageType"
    type: number
  - position: 2
    name: "message"
    type: string
  - position: 3
    name: "subsitution1"
    type: string
    default: "NULL"
  - position: 4
    name: "subsitution2"
    type: string
    default: "NULL"
  - position: 5
    name: "subsitution3"
    type: string
    default: "NULL"
  - position: 6
    name: "subsitution4"
    type: string
    default: "NULL"
  returns:
    
---

<div class="lua__search__keywords">
Players.ClientPrintToAll &#x2013; Print text to all clients, optionally substituting 4 strings. The type decides where to show it: HUD_PRINTCENTER (1), HUD_PRINTNOTIFY (2), HUD_PRINTTALK (3), HUD_PRINTCONSOLE (4) - Enums don't exist yet
</div>
