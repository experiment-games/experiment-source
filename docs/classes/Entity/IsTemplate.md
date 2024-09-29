---
template: lua-class-function.html
title: IsTemplate
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: IsTemplate
  realm: server
  description: "Wheter this entity is a template. Template entities are marked as templates by the level designer. They do not spawn, but their keyvalues are saved for use by a template spawner."
  
  arguments:
  - position: 1
    name: "entity"
    type: Entity
  returns:
    - type: boolean
      description: "true if template, false otherwise."
---

<div class="lua__search__keywords">
Entity:IsTemplate &#x2013; Wheter this entity is a template. Template entities are marked as templates by the level designer. They do not spawn, but their keyvalues are saved for use by a template spawner.
</div>
