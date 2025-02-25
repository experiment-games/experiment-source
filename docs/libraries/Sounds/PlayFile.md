---
template: lua-library-function.html
title: PlayFile
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Sounds
  function: PlayFile
  realm: client
  description: "Plays a sound from a file."
  
  arguments:
  - position: 1
    name: "file"
    type: string
  - position: 2
    name: "flags"
    type: integer
    default: "PLAY_SOUND_FLAG::STREAM_BLOCK"
  returns:
    
---

<div class="lua__search__keywords">
Sounds.PlayFile &#x2013; Plays a sound from a file.
</div>
