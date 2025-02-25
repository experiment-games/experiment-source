---
template: lua-library-function.html
title: Create
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: TakeDamageInfos
  function: Create
  realm: shared
  description: "Creates a new TakeDamageInfo object"
  
  arguments:
  - position: 1
    types:
      - name: "inflictor"
        type: Entity
      - name: "inflictor"
        type: Entity
      - name: "inflictor"
        type: Entity
      - name: "inflictor"
        type: Entity
  - position: 2
    types:
      - name: "attacker"
        type: Entity
      - name: "attacker"
        type: Entity
      - name: "attacker"
        type: Entity
      - name: "attacker"
        type: Entity
  - position: 3
    types:
      - name: "damage"
        type: number
      - name: "weapon"
        type: Entity
      - name: "damageForce"
        type: Vector
      - name: "weapon"
        type: Entity
  - position: 4
    types:
      - name: "damageType"
        type: number
      - name: "damage"
        type: number
      - name: "damagePosition"
        type: Vector
      - name: "damageForce"
        type: Vector
  - position: 5
    types:
      - name: "damageType"
        type: number
      - name: "damage"
        type: number
      - name: "damagePosition"
        type: Vector
      - name: "killType"
        type: number
    default: "0"
  - position: 6
    types:
      - name: "damageType"
        type: number
      - name: "damage"
        type: number
      - name: "killType"
        type: number
    default: "0"
  - position: 7
    types:
      - name: "damageType"
        type: number
      - name: "killType"
        type: number
    default: "0"
  - position: 8
    types:
      - name: "reportedPosition"
        type: Vector
    default: "NULL"
      - name: "killType"
        type: number
    default: "0"
  - position: 9
    name: "reportedPosition"
    type: Vector
    default: "NULL"
  returns:
    - type: TakeDamageInfo
      description: "The new TakeDamageInfo object."
---

<div class="lua__search__keywords">
TakeDamageInfos.Create &#x2013; Creates a new TakeDamageInfo object
</div>
