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
  library: RecipientFilters
  function: Create
  realm: shared
  description: "Creates a new recipient filter"
  
  arguments:
  - position: 1
    name: "isUnreliable"
    type: boolean
    default: "false"
  returns:
    - type: RecipientFilter
      description: "The new recipient filter"
---

<div class="lua__search__keywords">
RecipientFilters.Create &#x2013; Creates a new recipient filter
</div>
