---
template: lua-class-function.html
title: RequestInfo
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Panel
  function: RequestInfo
  realm: client
  description: "OutputData - keyName is the name of the attribute requested. for Labels 'text' is an option that returns the default text image text returns true on success in finding the requested value. false on failure."
  
  arguments:
  - position: 1
    name: "panel"
    type: Panel
  - position: 2
    name: "outputData"
    type: KeyValuesHandle
  returns:
    - type: boolean
      description: "Whether the info was requested"
---

<div class="lua__search__keywords">
Panel:RequestInfo &#x2013; OutputData - keyName is the name of the attribute requested. for Labels 'text' is an option that returns the default text image text returns true on success in finding the requested value. false on failure.
</div>
