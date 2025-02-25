---
template: lua-hook.html
title: PlayerSay
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  function: PlayerSay
  realm: server
  description: ""
  arguments:
    - name: "speaker"
      description: "The player who is speaking"
      type: Player
    - name: "chatMessage"
      description: ""
      type: string
    - name: "isTeamOnly"
      description: ""
      type: boolean
  returns:
    - type: string
      description: "return a replacement string or an empty string to block the message"
---

<div class="lua__search__keywords">
hook PlayerSay &#x2013; ""
</div>
