---
template: lua-library-function.html
title: CreatePopup
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Surfaces
  function: CreatePopup
  realm: client
  description: "Creates a popup."
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "isMinimized"
    type: boolean
  - position: 3
    name: "shouldShowTaskbarIcon"
    type: boolean
    default: "true"
  - position: 4
    name: "isDisabled"
    type: boolean
    default: "false"
  - position: 5
    name: "hasMouseInput"
    type: boolean
    default: "true"
  - position: 6
    name: "hasKeyboardInput"
    type: boolean
    default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Surfaces.CreatePopup &#x2013; Creates a popup.
</div>
