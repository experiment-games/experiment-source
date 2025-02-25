---
template: lua-library-function.html
title: LoadGameState
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: LoadGameState
  realm: server
  description: "Load the game state."
  
  arguments:
  - position: 1
    name: "fileName"
    type: string
  - position: 2
    name: "isDemo"
    type: boolean
    default: "false"
  returns:
    - type: boolean
      description: "True if the game state was loaded, false otherwise."
---

<div class="lua__search__keywords">
Engines.LoadGameState &#x2013; Load the game state.
</div>
