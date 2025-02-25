---
template: lua-library-function.html
title: PreEntityPacketReceived
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Predictions
  function: PreEntityPacketReceived
  realm: client
  description: "Call before the game receives an entity packet."
  
  arguments:
  - position: 1
    name: "commandsAcknowledged"
    type: number
  - position: 2
    name: "currentWorldUpdate"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Predictions.PreEntityPacketReceived &#x2013; Call before the game receives an entity packet.
</div>
