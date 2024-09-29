---
template: lua-library-function.html
title: IsBoxInViewCluster
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: IsBoxInViewCluster
  realm: client
  description: "Check if a box is in a view cluster."
  
  arguments:
  - position: 1
    name: "boxMin"
    type: Vector
  - position: 2
    name: "boxMax"
    type: Vector
  returns:
    - type: boolean
      description: "True if the box is in the view cluster, false otherwise."
---

<div class="lua__search__keywords">
Engines.IsBoxInViewCluster &#x2013; Check if a box is in a view cluster.
</div>
