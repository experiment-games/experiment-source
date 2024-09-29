---
template: lua-library-function.html
title: PushView3D
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Renders
  function: PushView3D
  realm: client
  description: "Push a 3D view."
  
  arguments:
  - position: 1
    name: "origin"
    type: Vector
    default: "&playerView.origin"
  - position: 2
    name: "angles"
    type: Angle
    default: "&playerView.angles"
  - position: 3
    name: "fov"
    type: number
    default: "playerView.fov"
  - position: 4
    name: "x"
    type: number
    default: "playerView.x"
  - position: 5
    name: "y"
    type: number
    default: "playerView.y"
  - position: 6
    name: "width"
    type: number
    default: "playerView.width"
  - position: 7
    name: "height"
    type: number
    default: "playerView.height"
  - position: 8
    name: "zNear"
    type: number
    default: "playerView.zNear"
  - position: 9
    name: "zFar"
    type: number
    default: "playerView.zFar"
  returns:
    
---

<div class="lua__search__keywords">
Renders.PushView3D &#x2013; Push a 3D view.
</div>
