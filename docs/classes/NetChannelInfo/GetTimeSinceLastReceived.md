---
template: lua-class-function.html
title: GetTimeSinceLastReceived
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetTimeSinceLastReceived
  realm: server
  description: "Returns the time since the last received packet."
  
  arguments:
  - position: 1
    name: "netChannel"
    type: NetChannelInfo
  returns:
    - type: number
      description: "The time since the last received packet."
---

<div class="lua__search__keywords">
NetChannelInfo:GetTimeSinceLastReceived &#x2013; Returns the time since the last received packet.
</div>
