---
template: lua-library-function.html
title: PrecacheDecal
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: PrecacheDecal
  realm: server
  description: "Precache a decal."
  
  arguments:
    - name: "decalName"
      type: string
    - name: "preload"
      type: boolean
      default: "false"
  returns:
    - type: integer
      description: "The decal index."
---

<div class="lua__search__keywords">
Engines.PrecacheDecal &#x2013; Precache a decal.
</div>
