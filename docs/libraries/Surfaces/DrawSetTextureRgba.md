---
template: lua-library-function.html
title: DrawSetTextureRgba
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: DrawSetTextureRgba
  realm: client
  description: "Sets the texture raw RGBA bytes"
  
  arguments:
  - position: 1
    name: "textureId"
    type: number
  - position: 2
    name: "rgba"
    type: string
  - position: 3
    name: "width"
    type: number
  - position: 4
    name: "height"
    type: number
  - position: 5
    name: "hardwareFilter"
    type: number
    default: "0"
  - position: 6
    name: "shouldForceReload"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Surfaces.DrawSetTextureRgba &#x2013; Sets the texture raw RGBA bytes
</div>
