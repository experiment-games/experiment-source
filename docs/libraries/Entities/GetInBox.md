---
template: lua-library-function.html
title: GetInBox
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: GetInBox
  realm: shared
  description: "Finds all entities in the given box. Note that clientPartitionMask is only available on the client."
  
  arguments:
    - name: "mins"
      type: Vector
    - name: "maxs"
      type: Vector
    - name: "flagMask"
      type: integer
      default: "PARTITION_CLIENT_NON_STATIC_EDICTS"
    - name: "clientPartitionMask"
      type: integer
      default: "PARTITION_CLIENT_NON_STATIC_EDICTS"
  returns:
    - type: table
      description: "A table of entities found."
    - type: number
      description: "The number of entities found."
---

<div class="lua__search__keywords">
Entities.GetInBox &#x2013; Finds all entities in the given box. Note that clientPartitionMask is only available on the client.
</div>
