---
template: lua-class-function.html
title: GetNetworkDataValue
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: GetNetworkDataValue
  realm: shared
  description: "Gets a data table variable of the given type, in the given slot."
  
  arguments:
    - name: "type"
      type: enumeration/NETWORK_VARIABLE_TYPE
    - name: "slot"
      type: number
  returns:
    - type: any
      description: "The value of the network variable"
---

<div class="lua__search__keywords">
Entity:GetNetworkDataValue &#x2013; Gets a data table variable of the given type, in the given slot.
</div>
