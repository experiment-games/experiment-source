---
template: lua-library-function.html
title: ReportFlagsChanged
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: ReportFlagsChanged
  realm: server
  description: "Reports that an entity's flags have changed"
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  - position: 2
    name: "flagsOld"
    type: number
  - position: 3
    name: "flagsNow"
    type: number
  returns:
    
---

<div class="lua__search__keywords">
Entities.ReportFlagsChanged &#x2013; Reports that an entity's flags have changed
</div>
