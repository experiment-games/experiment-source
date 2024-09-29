---
template: lua-library-function.html
title: InternalSendToClients
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Networks
  function: InternalSendToClients
  realm: server
  description: "Send a message to the specified clients, or all"
  
  arguments:
  - position: 1
    name: "messageTypeId"
    type: number
  - position: 2
    name: "messageWriter"
    type: MessageWriter
  - position: 3
    types:
      - name: "clientOrClients"
        type: table
    nillable: true
      - name: "clientOrClients"
        type: Player
    nillable: true
  returns:
    
---

<div class="lua__search__keywords">
Networks.InternalSendToClients &#x2013; Send a message to the specified clients, or all
</div>
