---
template: lua-library-function.html
title: Start
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: UserMessages
  function: Start
  realm: server
  description: "Starts a user message."
  
  arguments:
    - name: "messageName"
      type: string
    - name: "recipientFilter"
      type: RecipientFilter
      nillable: true
  returns:
    
---

<div class="lua__search__keywords">
UserMessages.Start &#x2013; Starts a user message.
</div>