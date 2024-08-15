---
template: lua-class-function.html
title: IsValidPacket
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: IsValidPacket
  realm: server
  description: "Returns if the packet is valid for the specified flow."
  
  arguments:
    - name: "flow"
      type: number
    - name: "frameNumber"
      type: number
  returns:
    - type: boolean
      description: "If the packet is valid for the specified flow."
---

<div class="lua__search__keywords">
NetChannelInfo:IsValidPacket &#x2013; Returns if the packet is valid for the specified flow.
</div>
