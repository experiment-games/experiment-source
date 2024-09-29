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
  - position: 1
    name: "netChannel"
    type: NetChannelInfo
  - position: 2
    name: "flow"
    type: number
  - position: 3
    name: "frameNumber"
    type: number
  - position: 4
    name: "group"
    type: number
  returns:
    - type: integer
      description: "The number of bytes in the specified packet."
---

<div class="lua__search__keywords">
NetChannelInfo:GetPacketBytes &#x2013; Returns the number of bytes in the specified packet.
</div>
