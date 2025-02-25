---
template: lua-library-function.html
title: PrecacheGeneric
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: PrecacheGeneric
  realm: server
  description: "Precache a generic."
  
  arguments:
  - position: 1
    name: "genericName"
    type: string
  - position: 2
    name: "preload"
    type: boolean
    default: "false"
  returns:
    - type: integer
      description: "The generic index."
---

<div class="lua__search__keywords">
Engines.PrecacheGeneric &#x2013; Precache a generic.
</div>
