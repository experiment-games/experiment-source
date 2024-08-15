---
template: lua-class-function.html
title: GetPacketBytes
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetPacketBytes
  realm: server
  description: "Returns the number of bytes in the specified packet."
  
  arguments:
    - name: "flow"
      type: number
    - name: "frameNumber"
      type: number
    - name: "group"
      type: number
  returns:
    - type: integer
      description: "The number of bytes in the specified packet."
---

<div class="lua__search__keywords">
NetChannelInfo:GetPacketBytes &#x2013; Returns the number of bytes in the specified packet.
</div>
