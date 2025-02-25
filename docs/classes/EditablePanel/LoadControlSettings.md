---
template: lua-class-function.html
title: LoadControlSettings
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: EditablePanel
  function: LoadControlSettings
  realm: client
  description: "Loads control settings"
  
  arguments:
  - position: 1
    name: "editablePanel"
    type: EditablePanel
  - position: 2
    name: "dialogResourceName"
    type: string
  - position: 3
    name: "pathId"
    type: string
    default: "NULL"
  - position: 4
    name: "keyValues"
    type: KeyValuesHandle
    default: "NULL"
  returns:
    
---

<div class="lua__search__keywords">
EditablePanel:LoadControlSettings &#x2013; Loads control settings
</div>
