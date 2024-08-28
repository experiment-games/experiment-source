---
template: lua-hook.html
title: CheckPassword
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  function: CheckPassword
  realm: server
  description: "Called to check if the password the user provides is correct."
  arguments:
    - name: "steamId64"
      description: ""
      type: string
    - name: "ipAddress"
      description: ""
      type: string
    - name: "actualPassword"
      description: ""
      type: string
    - name: "password"
      description: ""
      type: string
    - name: "name"
      description: ""
      type: string
  returns:
    - type: boolean
      description: "is the client allowed to join"
    - type: string
      description: "reason why the client is not allowed to join"
---

<div class="lua__search__keywords">
hook CheckPassword &#x2013; "Called to check if the password the user provides is correct."
</div>
