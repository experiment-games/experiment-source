---
template: lua-library-function.html
title: GetInSphere
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Entities
  function: GetInSphere
  realm: shared
  description: "Finds all entities in the given sphere. Note that clientPartitionMask is only available on the client."
  
  arguments:
    - name: "position"
      type: Vector
    - name: "radius"
      type: number
    - name: "flagMask"
      type: integer
      default: "PARTITION_CLIENT_NON_STATIC_EDICTS"
    - name: "partitionMask"
      type: integer
      default: "PARTITION_CLIENT_NON_STATIC_EDICTS"
  returns:
    - type: table
      description: "A table of entities found."
    - type: number
      description: "The number of entities found."
---

<div class="lua__search__keywords">
Entities.GetInSphere &#x2013; Finds all entities in the given sphere. Note that clientPartitionMask is only available on the client.
</div>
