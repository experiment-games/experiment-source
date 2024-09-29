---
template: lua-class-function.html
title: GetChild
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: GetChild
  realm: client
  description: "Gets a child panel by its index (starting at 0)"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "zeroBasedIndex"
    type: number
  returns:
    - type: Panel
      description: "The child panel"
---

<div class="lua__search__keywords">
Panel:GetChild &#x2013; Gets a child panel by its index (starting at 0)
</div>
