---
template: lua-class-function.html
title: GetSequenceNumber
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetSequenceNumber
  realm: server
  description: "Returns the sequence number for the specified flow."
  
  arguments:
  - position: 1
    name: "netChannel"
    type: NetChannelInfo
  - position: 2
    name: "flow"
    type: number
  returns:
    - type: integer
      description: "The sequence number for the specified flow."
---

<div class="lua__search__keywords">
NetChannelInfo:GetSequenceNumber &#x2013; Returns the sequence number for the specified flow.
</div>
