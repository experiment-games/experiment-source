---
template: lua-library-function.html
title: LoadSchemeFromFileEx
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Schemes
  function: LoadSchemeFromFileEx
  realm: client
  description: "Loads the scheme from the specified file."
  
  arguments:
  - position: 1
    name: "sizingPanel"
    type: Panel
  - position: 2
    name: "fileName"
    type: string
  - position: 3
    name: "tag"
    type: string
  returns:
    - type: scheme
      description: "The scheme loaded from the specified file."
---

<div class="lua__search__keywords">
Schemes.LoadSchemeFromFileEx &#x2013; Loads the scheme from the specified file.
</div>
