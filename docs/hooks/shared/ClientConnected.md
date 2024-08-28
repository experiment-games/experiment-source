---
template: lua-hook.html
title: ClientConnected
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  function: ClientConnected
  realm: shared
  description: "Called every time a client connects to the server. Return false to reject the connection (with an optional reject message)."
  arguments:
    - name: "name"
      description: ""
      type: string
    - name: "ipAddress"
      description: ""
      type: string
  returns:
    - type: boolean
      description: "should accept connection"
    - type: string
      description: "reject message"
---

<div class="lua__search__keywords">
hook ClientConnected &#x2013; "Called every time a client connects to the server. Return false to reject the connection (with an optional reject message)."
</div>
