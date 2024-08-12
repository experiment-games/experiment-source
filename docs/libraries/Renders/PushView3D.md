---
template: lua-library-function.html
title: PushView3D
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Renders
  function: PushView3D
  realm: shared
  description: "Push a 3D view."
  
  arguments:
    - name: "origin"
      type: Vector
      default: "&playerView.origin"
    - name: "angles"
      type: Angle
      default: "&playerView.angles"
    - name: "fov"
      type: number
      default: "playerView.fov"
    - name: "x"
      type: number
      default: "playerView.x"
    - name: "y"
      type: number
      default: "playerView.y"
    - name: "width"
      type: number
      default: "playerView.width"
    - name: "height"
      type: number
      default: "playerView.height"
    - name: "zNear"
      type: number
      default: "playerView.zNear"
    - name: "zFar"
      type: number
      default: "playerView.zFar"
  returns:
    
---

<div class="lua__search__keywords">
Renders.PushView3D &#x2013; Push a 3D view.
</div>
