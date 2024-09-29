---
template: lua-library-function.html
title: SayTextToAll
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Players
  function: SayTextToAll
  realm: shared
  description: "Say text to all players."
  
  arguments:
  - position: 1
    name: "text"
    type: string
  - position: 2
    name: "speakingPlayer"
    type: Player
    default: "0"
  - position: 3
    name: "withoutStringLookup"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Players.SayTextToAll &#x2013; Say text to all players.
</div>
