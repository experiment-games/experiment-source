---
template: lua-hook.html
title: CalcViewModelView
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  function: CalcViewModelView
  realm: shared
  description: "Allows overriding the view model's position and angles."
  arguments:
    - name: "weapon"
      description: ""
      type: Entity
    - name: "viewModel"
      description: ""
      type: Entity
    - name: "eyePosition"
      description: ""
      type: Vector
    - name: "eyeAngles"
      description: ""
      type: Angle
    - name: "viewModelOrigin"
      description: ""
      type: Vector
    - name: "viewModelAngles"
      description: ""
      type: Angle
  returns:
    - type: Vector
      description: "replacement origin"
    - type: Angle
      description: "replacement angles"
---

<div class="lua__search__keywords">
hook CalcViewModelView &#x2013; "Allows overriding the view model's position and angles."
</div>
