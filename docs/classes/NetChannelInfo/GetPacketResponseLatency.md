---
template: lua-class-function.html
title: GetPacketResponseLatency
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetPacketResponseLatency
  realm: server
  description: "Returns the packet response latency for the specified packet and flow."
  
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
    - type: integer
      description: "The packet response latency"
    - type: integer
      description: "The choke."
---

<div class="lua__search__keywords">
NetChannelInfo:GetPacketResponseLatency &#x2013; Returns the packet response latency for the specified packet and flow.
</div>
