---
template: lua-class-function.html
title: GetInset
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: GetInset
  realm: client
  description: "Gets the inset of the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  returns:
    - type: integer
      description: "The left inset of the panel"
    - type: integer
      description: "The top inset of the panel"
    - type: integer
      description: "The right inset of the panel"
    - type: integer
      description: "The bottom inset of the panel"
---

<div class="lua__search__keywords">
Panel:GetInset &#x2013; Gets the inset of the panel
</div>
