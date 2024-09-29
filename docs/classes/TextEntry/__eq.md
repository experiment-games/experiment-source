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
  library: TextEntry
  function: __eq
  realm: client
  description: "Metamethod called when two TextEntry objects are compared"
  
  arguments:
  - position: 1
    name: "textEntry"
    type: TextEntry
  - position: 2
    name: "other"
    type: TextEntry
  returns:
    - type: boolean
      description: "Whether the TextEntry objects are equal"
---

<div class="lua__search__keywords">
TextEntry:__eq &#x2013; Metamethod called when two TextEntry objects are compared
</div>
