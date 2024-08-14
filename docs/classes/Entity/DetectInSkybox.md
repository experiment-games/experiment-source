---
template: lua-class-function.html
title: DetectInSkybox
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: DetectInSkybox
  realm: server
  description: "This function finds out if the entity is in the 3D skybox. If so, it sets the EFL_IN_SKYBOX flag so the entity gets transmitted to all the clients. Entities usually call this during their Activate(). Returns true if the entity is in the skybox (and EFL_IN_SKYBOX was set).."
  
  
  returns:
    - type: boolean
      description: "true if in skybox, false otherwise."
---

<div class="lua__search__keywords">
Entity:DetectInSkybox &#x2013; This function finds out if the entity is in the 3D skybox. If so, it sets the EFL_IN_SKYBOX flag so the entity gets transmitted to all the clients. Entities usually call this during their Activate(). Returns true if the entity is in the skybox (and EFL_IN_SKYBOX was set)..
</div>
