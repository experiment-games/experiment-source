---
template: lua-library-function.html
title: PrecacheSentenceFile
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Engines
  function: PrecacheSentenceFile
  realm: server
  description: "Precache a sentence file."
  
  arguments:
    - name: "sentenceFileName"
      type: string
    - name: "preload"
      type: boolean
      default: "false"
  returns:
    - type: integer
      description: "The sentence file index."
---

<div class="lua__search__keywords">
Engines.PrecacheSentenceFile &#x2013; Precache a sentence file.
</div>
