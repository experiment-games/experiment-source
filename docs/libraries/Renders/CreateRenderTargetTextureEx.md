---
template: lua-library-function.html
title: CreateRenderTargetTextureEx
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Renders
  function: CreateRenderTargetTextureEx
  realm: shared
  description: "Create a new render target texture."
  
  arguments:
    - name: "name"
      type: string
    - name: "width"
      type: number
    - name: "height"
      type: number
    - name: "sizeMode"
      type: number
      default: "RT_SIZE_DEFAULT"
    - name: "depthMode"
      type: number
      default: "MATERIAL_RT_DEPTH_SHARED"
    - name: "textureFlags"
      type: number
      default: "TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT"
    - name: "renderTargetFlags"
      type: number
      default: "0"
    - name: "imageFormat"
      type: number
      default: "IMAGE_FORMAT_RGBA8888"
  returns:
    - type: Texture
      description: "The created render target texture."
---

<div class="lua__search__keywords">
Renders.CreateRenderTargetTextureEx &#x2013; Create a new render target texture.
</div>
