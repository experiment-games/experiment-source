---
template: lua-class-function.html
title: __eq
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Button
  function: __eq
  realm: client
  description: "Metamethod that is called when two buttons are compared"
  
  arguments:
  - position: 1
    name: "label"
    type: Button
  - position: 2
    name: "other"
    type: Button
  returns:
    - type: boolean
      description: "Whether the labels are equal"
---

<div class="lua__search__keywords">
Button:__eq &#x2013; Metamethod that is called when two buttons are compared
</div>
