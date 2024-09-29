---
template: lua-class-function.html
title: GetAveragePackets
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetAveragePackets
  realm: server
  description: "Returns the average packets for the specified number of packets."
  
  arguments:
  - position: 1
    name: "netChannel"
    type: NetChannelInfo
  - position: 2
    name: "flow"
    type: number
  returns:
    - type: number
      description: "The average packets for the specified number of packets."
---

<div class="lua__search__keywords">
NetChannelInfo:GetAveragePackets &#x2013; Returns the average packets for the specified number of packets.
</div>
