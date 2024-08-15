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
    - name: "textureId"
      type: number
    - name: "rgba"
      type: string
    - name: "width"
      type: number
    - name: "height"
      type: number
    - name: "hardwareFilter"
      type: number
      default: "0"
    - name: "shouldForceReload"
      type: boolean
      default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Surfaces.DrawSetTextureRgba &#x2013; Sets the texture raw RGBA bytes
</div>
