---
template: lua-class-function.html
title: GetDockMargin
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: GetDockMargin
  realm: client
  description: "Gets the dock margin of the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  returns:
    - type: integer
      description: "The left margin of the dock"
    - type: integer
      description: "The top margin of the dock"
    - type: integer
      description: "The right margin of the dock"
    - type: integer
      description: "The bottom margin of the dock"
---

<div class="lua__search__keywords">
Panel:GetDockMargin &#x2013; Gets the dock margin of the panel
</div>
