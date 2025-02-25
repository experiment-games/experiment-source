---
template: lua-class-function.html
title: GetPacketTime
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetPacketTime
  realm: server
  description: "Returns the time of the specified packet and flow."
  
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
  returns:
    - type: number
      description: "The time of the specified packet and flow."
---

<div class="lua__search__keywords">
NetChannelInfo:GetPacketTime &#x2013; Returns the time of the specified packet and flow.
</div>
