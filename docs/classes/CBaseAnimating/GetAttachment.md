---
template: lua-class-function.html
title: GetAttachment
lua:
  library: CBaseAnimating
  function: GetAttachment
  realm: shared
  description: Get the attachment table for the specified attachment (by bone id or attachment name)
  argumentSets:
    - arguments:
      - name: boneId
        type: number
        description: undefined
    - arguments:
      - name: attachmentName
        type: string
        description: undefined
  returns:
    - type: table
      description: The attachment information
---
