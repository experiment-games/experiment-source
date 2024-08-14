---
template: lua-library-function.html
title: Base64Encode
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: Serializers
  function: Base64Encode
  realm: shared
  description: "Encode a string to base64"
  
  arguments:
    - name: "string"
      type: string
    - name: "disableRfc2045Compliance"
      type: boolean
      default: "false"
  returns:
    - type: string
      description: "Base64 encoded string."
---

<div class="lua__search__keywords">
Serializers.Base64Encode &#x2013; Encode a string to base64
</div>
