---
template: lua-library-function.html
title: ClientsideModel
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: _G
  function: ClientsideModel
  realm: client
  description: "Create a clientside model."
  
  arguments:
    - name: "model"
      type: string
    - name: "renderGroup"
      type: number
      default: "RENDER_GROUP_OTHER"
  returns:
    - type: Entity
      description: "Clientside model entity."
---

<div class="lua__search__keywords">
_G.ClientsideModel &#x2013; Create a clientside model.
</div>
