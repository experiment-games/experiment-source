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
  library: Utilities
  function: PrecacheDecal
  realm: server
  description: "Precaches a decal. If preload is specified, the file is loaded into the server/client's cache memory before level startup, otherwise it'll only load when actually used (which can cause a disk i/o hitch if it occurs during play of a level)."
  
  arguments:
    - name: "name"
      type: string
    - name: "shouldPreload"
      type: boolean
      default: "false"
  returns:
    - type: number
      description: "The decal index."
---

<div class="lua__search__keywords">
Utilities.PrecacheDecal &#x2013; Precaches a decal. If preload is specified, the file is loaded into the server/client's cache memory before level startup, otherwise it'll only load when actually used (which can cause a disk i/o hitch if it occurs during play of a level).
</div>
