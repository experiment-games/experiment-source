---
template: lua-class-function.html
title: FindChildByName
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: FindChildByName
  realm: client
  description: "Finds a child panel by its name"
  
  arguments:
    - name: "name"
      type: string
    - name: "shouldSearchRecursive"
      type: boolean
      default: "false"
  returns:
    - type: Panel
      description: "The child panel"
---

<div class="lua__search__keywords">
Panel:FindChildByName &#x2013; Finds a child panel by its name
</div>
