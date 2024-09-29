---
template: lua-library-function.html
title: LookupBinding
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Inputs
  function: LookupBinding
  realm: client
  description: "Look up a key binding."
  
  arguments:
  - position: 1
    name: "keyName"
    type: string
  - position: 2
    name: "exactMatch"
    type: boolean
  returns:
    - type: string
      description: "The key binding."
---

<div class="lua__search__keywords">
Inputs.LookupBinding &#x2013; Look up a key binding.
</div>
