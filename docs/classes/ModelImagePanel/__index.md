---
template: lua-class-function.html
title: __index
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: ModelImagePanel
  function: __index
  realm: client
  description: "Metamethod called when a non-existent field is indexed"
  
  arguments:
  - position: 1
    name: "modelImagePanel"
    type: ModelImagePanel
  - position: 2
    name: "field"
    type: string
  returns:
    - type: any
      description: "The value of the field"
---

<div class="lua__search__keywords">
ModelImagePanel:__index &#x2013; Metamethod called when a non-existent field is indexed
</div>
