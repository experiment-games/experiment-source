---
template: lua-library-function.html
title: DrawSetTextureFile
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: DrawSetTextureFile
  realm: client
  description: "Sets the texture file"
  
  arguments:
  - position: 1
    name: "textureId"
    type: number
  - position: 2
    name: "texturePath"
    type: string
  - position: 3
    name: "hardwareFilter"
    type: number
    default: "0"
  - position: 4
    name: "shouldForceReload"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Surfaces.DrawSetTextureFile &#x2013; Sets the texture file
</div>
