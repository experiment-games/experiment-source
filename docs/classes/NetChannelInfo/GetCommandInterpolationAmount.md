---
template: lua-class-function.html
title: GetCommandInterpolationAmount
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetCommandInterpolationAmount
  realm: server
  description: "Returns the command interpolation amount for the specified command number."
  
  arguments:
  - position: 1
    name: "netChannel"
    type: NetChannelInfo
  - position: 2
    name: "commandNumber"
    type: number
  - position: 3
    name: "flow"
    type: number
  returns:
    - type: number
      description: "The command interpolation amount for the specified command number."
---

<div class="lua__search__keywords">
NetChannelInfo:GetCommandInterpolationAmount &#x2013; Returns the command interpolation amount for the specified command number.
</div>
