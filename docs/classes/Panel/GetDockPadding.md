---
template: lua-class-function.html
title: GetDockPadding
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: GetDockPadding
  realm: client
  description: "Gets the dock padding of the panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  returns:
    - type: integer
      description: "The left padding of the dock"
    - type: integer
      description: "The top padding of the dock"
    - type: integer
      description: "The right padding of the dock"
    - type: integer
      description: "The bottom padding of the dock"
---

<div class="lua__search__keywords">
Panel:GetDockPadding &#x2013; Gets the dock padding of the panel
</div>
