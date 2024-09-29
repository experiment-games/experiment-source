---
template: lua-library-function.html
title: RequestHttpMethod
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: WebConnections
  function: RequestHttpMethod
  realm: client
  description: "Performs an HTTP request of the specified method"
  
  arguments:
  - position: 1
    name: "url"
    type: string
  - position: 2
    name: "method"
    type: string
  - position: 3
    name: "body"
    type: string
    default: "nullptr"
  - position: 4
    name: "parameters"
    type: KeyValuesHandle
    nillable: true
  - position: 5
    name: "headers"
    type: KeyValuesHandle
    nillable: true
  - position: 6
    name: "contentType"
    type: string
    default: "\"text/plain; charset=utf-8\""
  - position: 7
    name: "timeout"
    type: number
    default: "60"
  returns:
    
---

<div class="lua__search__keywords">
WebConnections.RequestHttpMethod &#x2013; Performs an HTTP request of the specified method
</div>
