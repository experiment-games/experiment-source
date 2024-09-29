---
template: lua-library-function.html
title: PlayUrl
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Sounds
  function: PlayUrl
  realm: client
  description: "Plays a sound from a URL."
  
  arguments:
  - position: 1
    name: "url"
    type: string
  - position: 2
    name: "flags"
    type: integer
    default: "PLAY_SOUND_FLAG::STREAM_BLOCK"
  returns:
    
---

<div class="lua__search__keywords">
Sounds.PlayUrl &#x2013; Plays a sound from a URL.
</div>
