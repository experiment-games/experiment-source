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
    - name: "genericName"
      type: string
    - name: "preload"
      type: boolean
      default: "false"
  returns:
    - type: integer
      description: "The generic index."
---

<div class="lua__search__keywords">
Engines.PrecacheGeneric &#x2013; Precache a generic.
</div>
