---
template: lua-hook.html
title: PlayersCanPickupObject
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  function: PlayersCanPickupObject
  realm: server
  description: "Checks if any player can pickup this type of object."
  arguments:
    - name: "entity"
      description: "The entity that is being checked."
      type: Entity
    - name: "massLimit"
      description: "The mass limit of the object."
      type: number
    - name: "sizeLimit"
      description: "The size limit of the object."
      type: number
  returns:
    - type: boolean
      description: "Whether or not the object can be picked up."
---

<div class="lua__search__keywords">
hook PlayersCanPickupObject &#x2013; "Checks if any player can pickup this type of object."
</div>
