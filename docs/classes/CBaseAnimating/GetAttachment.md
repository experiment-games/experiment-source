---
template: lua-class-function.html
title: GetAttachment
icon: lua-shared
lua:
  library: CBaseAnimating
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
