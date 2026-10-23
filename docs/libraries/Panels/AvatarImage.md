---
template: lua-library-function.html
title: AvatarImage
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panels
  function: AvatarImage
  realm: client
  description: "Creates a new avatarImage"
  
  arguments:
  - position: 1
    name: "parent"
    type: Panel
    default: "VGui_GetClientLuaRootPanel()"
  - position: 2
    name: "name"
    type: string
    default: "\"AvatarImage\""
  returns:
    - type: AvatarImage
      description: "The new avatarImage"
---

<div class="lua__search__keywords">
Panels.AvatarImage &#x2013; Creates a new avatarImage
</div>
