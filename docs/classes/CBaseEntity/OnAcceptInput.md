---
template: lua-class-function.html
title: OnAcceptInput
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: CBaseEntity
  function: OnAcceptInput
  realm: server
  description: "Called when the entity accepted input"
  
  arguments:
    - name: "input"
      type: string
    - name: "activator"
      type: Entity
    - name: "caller"
      type: Entity
    - name: "outputID"
      type: number
  returns:
    - type: boolean
      description: "true if accepted, false otherwise."
---

<div class="lua__search__keywords">
CBaseEntity:OnAcceptInput &#x2013; Called when the entity accepted input
</div>
