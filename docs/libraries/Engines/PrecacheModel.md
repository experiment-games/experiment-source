---
template: lua-library-function.html
title: PrecacheModel
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: PrecacheModel
  realm: server
  description: "Precache a model."
  
  arguments:
  - position: 1
    name: "modelName"
    type: string
  - position: 2
    name: "preload"
    type: boolean
    default: "false"
  returns:
    - type: integer
      description: "The model index."
---

<div class="lua__search__keywords">
Engines.PrecacheModel &#x2013; Precache a model.
</div>
