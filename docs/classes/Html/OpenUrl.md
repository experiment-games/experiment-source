---
template: lua-class-function.html
title: OpenUrl
icon: lua-client
tags:
  - lua
  - client
  - needs-verification
  - needs-example
lua:
  library: Html
  function: OpenUrl
  realm: client
  description: "Opens a URL in the HTML object"
  
  arguments:
  - position: 1
    name: "html"
    type: Html
  - position: 2
    name: "url"
    type: string
  - position: 3
    name: "postData"
    type: string
    default: "\"\""
  - position: 4
    name: "allowJS"
    type: boolean
    default: "false"
  returns:
    
---

<div class="lua__search__keywords">
Html:OpenUrl &#x2013; Opens a URL in the HTML object
</div>
