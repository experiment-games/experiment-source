---
template: lua-class-function.html
title: GetAttachmentVelocity
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: CBaseAnimating
  function: GetAttachmentVelocity
  realm: client
  description: "Get the attachment velocity."
  arguments:
    - name: "attachment"
      type: number
  returns:
    - type: boolean
      description: "Whether the operation was successful"
    - type: vector
      description: "The origin velocity"
---
