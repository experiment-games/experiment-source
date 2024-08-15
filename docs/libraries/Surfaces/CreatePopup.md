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
    - name: "panel"
      type: Panel
    - name: "isMinimized"
      type: boolean
    - name: "shouldShowTaskbarIcon"
      type: boolean
      default: "true"
    - name: "isDisabled"
      type: boolean
      default: "false"
    - name: "hasMouseInput"
      type: boolean
      default: "true"
    - name: "hasKeyboardInput"
      type: boolean
      default: "true"
  returns:
    
---

<div class="lua__search__keywords">
Surfaces.CreatePopup &#x2013; Creates a popup.
</div>
