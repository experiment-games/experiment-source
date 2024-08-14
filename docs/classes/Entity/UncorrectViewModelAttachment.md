---
template: lua-class-function.html
title: UncorrectViewModelAttachment
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Entity
  function: UncorrectViewModelAttachment
  realm: client
  description: "All view model attachments origins are stretched so you can place entities at them an dthey will match up with where the attachment winds up being drawn on the view model, since the view models are drawn with a different FOV.\n\nIf you're drawing something inside of a view model's DrawModel() function, then you want the original attachment origin instead of the adjusted one. To get that, call this on the adjusted attachment origin. \nThis only works for C_BaseViewModel entities."
  
  arguments:
    - name: "origin"
      type: Vector
  returns:
    - type: Vector
      description: "The uncorrected view model attachment origin"
---

<div class="lua__search__keywords">
Entity:UncorrectViewModelAttachment &#x2013; All view model attachments origins are stretched so you can place entities at them an dthey will match up with where the attachment winds up being drawn on the view model, since the view models are drawn with a different FOV.\n\nIf you're drawing something inside of a view model's DrawModel() function, then you want the original attachment origin instead of the adjusted one. To get that, call this on the adjusted attachment origin. \nThis only works for C_BaseViewModel entities.
</div>
