---
template: lua-class-function.html
title: GetAttachmentLocal
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: CBaseAnimating
  function: GetAttachmentLocal
  realm: client
  description: "Get the local attachment."
  arguments:
    - name: "attachment"
      type: number
  returns:
    - type: boolean
      description: "Whether the operation was successful"
    - type: vector
      description: "The origin"
    - type: angle
      description: "The angles"
---
