---
template: lua-class-function.html
title: SetNetworkDataValue
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: SetNetworkDataValue
  realm: shared
  description: "Sets a data table variable of the given type, in the given slot, to the given value. Slots start at 0 and there's 32 slots per type (except for strings, which have 4 slots and a limit of 512 characters per string)."
  
  argumentSets:
    - arguments:
      - name: "type"
        type: enumeration/NETWORK_VARIABLE_TYPE
      - name: "slot"
        type: number
      - name: "value"
        type: boolean
    - arguments:
      - name: "value"
        type: number
    - arguments:
      - name: "value"
        type: number
    - arguments:
      - name: "value"
        type: Vector
    - arguments:
      - name: "value"
        type: Angle
    - arguments:
      - name: "value"
        type: string
    - arguments:
      - name: "value"
        type: Entity
  returns:
    
---

<div class="lua__search__keywords">
Entity:SetNetworkDataValue &#x2013; Sets a data table variable of the given type, in the given slot, to the given value. Slots start at 0 and there's 32 slots per type (except for strings, which have 4 slots and a limit of 512 characters per string).
</div>
