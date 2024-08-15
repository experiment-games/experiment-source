---
template: lua-class-function.html
title: GetStreamProgress
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetStreamProgress
  realm: server
  description: "Returns the stream progress for the specified flow."
  
  arguments:
    - name: "flow"
      type: number
  returns:
    - type: boolean
      description: "If the stream progress was retrieved."
    - type: integer
      description: "The number of bytes received."
    - type: integer
      description: "The total number of bytes."
---

<div class="lua__search__keywords">
NetChannelInfo:GetStreamProgress &#x2013; Returns the stream progress for the specified flow.
</div>
