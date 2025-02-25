---
template: lua-hook.html
title: KeyInput
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  function: KeyInput
  realm: client
  description: "Called when a key is pressed"
  arguments:
    - name: "isDown"
      description: ""
      type: number
    - name: "keyCode"
      description: ""
      type: number
    - name: "binding"
      description: "the binding related to the key"
      type: string
  returns:
    - type: unknown
      description: "boolean (return false to prevent the engine from handling the key"
    - type: unknown
      description: "true to allow and override default behavior)"
---

<div class="lua__search__keywords">
hook KeyInput &#x2013; "Called when a key is pressed"
</div>
