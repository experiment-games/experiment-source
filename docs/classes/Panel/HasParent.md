---
template: lua-class-function.html
title: HasParent
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: HasParent
  realm: client
  description: "Checks if the panel is a descendant of another panel"
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "potentialParent"
    type: Panel
  returns:
    - type: boolean
      description: "True if the panel is a descendant of the parent, false otherwise"
---

<div class="lua__search__keywords">
Panel:HasParent &#x2013; Checks if the panel is a descendant of another panel
</div>
