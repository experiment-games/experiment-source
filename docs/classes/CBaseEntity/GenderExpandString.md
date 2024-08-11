---
template: lua-class-function.html
title: GenderExpandString
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: CBaseEntity
  function: GenderExpandString
  realm: shared
  description: "Expands any $gender tags in the string into male or female tags based on the gender of the actor (based on his/her .mdl). Used for filesnames only. The returned string is at most 260 characters long."
  
  arguments:
    - name: "string"
      type: string
  returns:
    - type: string
      description: "The expanded string."
---

<div class="lua__search__keywords">
CBaseEntity:GenderExpandString &#x2013; Expands any $gender tags in the string into male or female tags based on the gender of the actor (based on his/her .mdl). Used for filesnames only. The returned string is at most 260 characters long.
</div>
