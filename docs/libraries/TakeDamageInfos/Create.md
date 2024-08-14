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
  
  argumentSets:
    - arguments:
      - name: "inflictor"
        type: Entity
      - name: "attacker"
        type: Entity
      - name: "damage"
        type: number
      - name: "damageType"
        type: number
      - name: "killType"
        type: number
        default: "0"
    - arguments:
      - name: "inflictor"
        type: Entity
      - name: "attacker"
        type: Entity
      - name: "weapon"
        type: Entity
      - name: "damage"
        type: number
      - name: "damageType"
        type: number
      - name: "killType"
        type: number
        default: "0"
    - arguments:
      - name: "inflictor"
        type: Entity
      - name: "attacker"
        type: Entity
      - name: "damageForce"
        type: Vector
      - name: "damagePosition"
        type: Vector
      - name: "damage"
        type: number
      - name: "damageType"
        type: number
      - name: "killType"
        type: number
        default: "0"
      - name: "reportedPosition"
        type: Vector
        default: "NULL"
    - arguments:
      - name: "inflictor"
        type: Entity
      - name: "attacker"
        type: Entity
      - name: "weapon"
        type: Entity
      - name: "damageForce"
        type: Vector
      - name: "damagePosition"
        type: Vector
      - name: "damage"
        type: number
      - name: "damageType"
        type: number
      - name: "killType"
        type: number
        default: "0"
      - name: "reportedPosition"
        type: Vector
        default: "NULL"
  returns:
    - type: TakeDamageInfo
      description: "The new TakeDamageInfo object."
---

<div class="lua__search__keywords">
TakeDamageInfos.Create &#x2013; Creates a new TakeDamageInfo object
</div>
