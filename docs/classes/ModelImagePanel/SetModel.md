---
template: lua-class-function.html
title: SetModel
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: ModelImagePanel
  function: SetModel
  realm: client
  description: "Sets the model for the panel"
  
  arguments:
  - position: 1
    name: "modelImagePanel"
    type: ModelImagePanel
  - position: 2
    name: "modelPath"
    type: string
  - position: 3
    name: "skin"
    type: number
    default: "0"
  - position: 4
    name: "bodyGroups"
    type: string
    default: "NULL"
  returns:
    
---

<div class="lua__search__keywords">
ModelImagePanel:SetModel &#x2013; Sets the model for the panel
</div>
