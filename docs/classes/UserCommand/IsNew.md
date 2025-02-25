---
template: lua-class-function.html
title: IsNew
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: UserCommand
  function: IsNew
  realm: shared
  description: "Whether this command is repeated (likely due to lag) and it isn't new."
  
  arguments:
  - position: 1
    name: "userCmd"
    type: UserCommand
  returns:
    - type: boolean
      description: "True if the command is new, false otherwise"
---

<div class="lua__search__keywords">
UserCommand:IsNew &#x2013; Whether this command is repeated (likely due to lag) and it isn't new.
</div>
