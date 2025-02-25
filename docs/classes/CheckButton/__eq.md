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
  library: CheckButton
  function: __eq
  realm: client
  description: "Metamethod that is called when two buttons are compared"
  
  arguments:
  - position: 1
    name: "checkButton"
    type: CheckButton
  - position: 2
    name: "other"
    type: CheckButton
  returns:
    - type: boolean
      description: "Whether the buttons are equal"
---

<div class="lua__search__keywords">
CheckButton:__eq &#x2013; Metamethod that is called when two buttons are compared
</div>
