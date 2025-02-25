---
template: lua-library-function.html
title: CreateRenderTargetTextureEx
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Renders
  function: CreateRenderTargetTextureEx
  realm: client
  description: "Create a new render target texture."
  
  arguments:
  - position: 1
    name: "name"
    type: string
  - position: 2
    name: "width"
    type: number
  - position: 3
    name: "height"
    type: number
  - position: 4
    name: "sizeMode"
    type: number
    default: "RT_SIZE_DEFAULT"
  - position: 5
    name: "depthMode"
    type: number
    default: "MATERIAL_RT_DEPTH_SHARED"
  - position: 6
    name: "textureFlags"
    type: number
    default: "TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT"
  - position: 7
    name: "renderTargetFlags"
    type: number
    default: "0"
  - position: 8
    name: "imageFormat"
    type: number
    default: "IMAGE_FORMAT_RGBA8888"
  returns:
    - type: Texture
      description: "The created render target texture."
---

<div class="lua__search__keywords">
Renders.CreateRenderTargetTextureEx &#x2013; Create a new render target texture.
</div>
