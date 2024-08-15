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
    - name: "messageType"
      type: number
    - name: "message"
      type: string
    - name: "subsitution1"
      type: string
      default: "NULL"
    - name: "subsitution2"
      type: string
      default: "NULL"
    - name: "subsitution3"
      type: string
      default: "NULL"
    - name: "subsitution4"
      type: string
      default: "NULL"
  returns:
    
---

<div class="lua__search__keywords">
Players.ClientPrintToAll &#x2013; Print text to all clients, optionally substituting 4 strings. The type decides where to show it: HUD_PRINTCENTER (1), HUD_PRINTNOTIFY (2), HUD_PRINTTALK (3), HUD_PRINTCONSOLE (4) - Enums don't exist yet
</div>
