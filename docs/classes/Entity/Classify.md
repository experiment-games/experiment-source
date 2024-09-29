---
template: lua-class-function.html
title: Classify
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: Classify
  realm: server
  description: "Returns the type of group (i.e, 'houndeye', or 'human military' so that NPCs with different classnames still realize that they are teammates. (overridden for NPCs that form groups)."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: enumeration/AI_CLASS
      description: "The classification"
---

<div class="lua__search__keywords">
Entity:Classify &#x2013; Returns the type of group (i.e, 'houndeye', or 'human military' so that NPCs with different classnames still realize that they are teammates. (overridden for NPCs that form groups).
</div>
