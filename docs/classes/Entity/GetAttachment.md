---
template: lua-class-function.html
title: GetAttachment
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetAttachment
  realm: shared
  description: "Get the attachment table for the specified attachment (by bone id or attachment name)"
  
  argumentSets:
    - arguments:
      - name: "boneId"
        type: number
    - arguments:
      - name: "attachmentName"
        type: string
  returns:
    - type: table
      description: "The attachment information"
---

<div class="lua__search__keywords">
Entity:GetAttachment &#x2013; Get the attachment table for the specified attachment (by bone id or attachment name)
</div>
