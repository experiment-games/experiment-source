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
  library: CheckButton
  function: __index
  realm: client
  description: "Metamethod that is called when a non-existent field is indexed"
  
  arguments:
  - position: 1
    name: "checkButton"
    type: CheckButton
  - position: 2
    name: "field"
    type: string
  returns:
    - type: any
      description: "The value of the field"
---

<div class="lua__search__keywords">
CheckButton:__index &#x2013; Metamethod that is called when a non-existent field is indexed
</div>
