---
template: lua-class-function.html
title: GetLatency
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetLatency
  realm: server
  description: "Returns the latency for the specified flow."
  
  arguments:
  - position: 1
    name: "netChannel"
    type: NetChannelInfo
  - position: 2
    name: "flow"
    type: number
  returns:
    - type: number
      description: "The latency for the specified flow."
---

<div class="lua__search__keywords">
NetChannelInfo:GetLatency &#x2013; Returns the latency for the specified flow.
</div>
