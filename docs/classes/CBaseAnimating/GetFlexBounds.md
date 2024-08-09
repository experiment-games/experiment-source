---
template: lua-class-function.html
title: GetFlexBounds
icon: lua-shared
lua:
  library: CBaseAnimating
  function: GetFlexBounds
  realm: shared
  description: "Returns the min and max values for the target flex controller"
  arguments:
    - name: "flexControllerIndex"
      type: number
  returns:
    - type: number
      description: "The min value"
    - type: number
      description: "The max value"
---
