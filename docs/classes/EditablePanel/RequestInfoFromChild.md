---
template: lua-class-function.html
title: RequestInfoFromChild
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: EditablePanel
  function: RequestInfoFromChild
  realm: client
  description: "Requests info from a child panel"
  
  arguments:
  - position: 1
    name: "editablePanel"
    type: EditablePanel
  - position: 2
    name: "childName"
    type: string
  - position: 3
    name: "keyValues"
    type: KeyValuesHandle
  returns:
    - type: boolean
      description: "True if the request was successful"
---

<div class="lua__search__keywords">
EditablePanel:RequestInfoFromChild &#x2013; Requests info from a child panel
</div>
