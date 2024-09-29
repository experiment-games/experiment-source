---
template: lua-class-function.html
title: GetAverageData
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: NetChannelInfo
  function: GetAverageData
  realm: server
  description: "Returns the average data for the specified number of packets."
  
  arguments:
  - position: 1
    name: "netChannel"
    type: NetChannelInfo
  - position: 2
    name: "flow"
    type: number
  returns:
    - type: number
      description: "The average data for the specified number of packets."
---

<div class="lua__search__keywords">
NetChannelInfo:GetAverageData &#x2013; Returns the average data for the specified number of packets.
</div>
