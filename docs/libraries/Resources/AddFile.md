---
template: lua-library-function.html
title: AddFile
icon: lua-server
tags:
  - lua
  - server
  - needs-verification
  - needs-example
lua:
  library: Resources
  function: AddFile
  realm: server
  description: "Add a file to the list of resources so it's downloaded when players connect to the server. For convenience .vmt and .mdl will also add the related .vtf/.png/.jpg and model files (.vvd, .ani, .dx80.vtx, .dx90.vtx, .sw.vtx, .phy) with the same name. It will warn for each missing file."
  
  arguments:
    - name: "filePath"
      type: string
  returns:
    
---

<div class="lua__search__keywords">
Resources.AddFile &#x2013; Add a file to the list of resources so it's downloaded when players connect to the server. For convenience .vmt and .mdl will also add the related .vtf/.png/.jpg and model files (.vvd, .ani, .dx80.vtx, .dx90.vtx, .sw.vtx, .phy) with the same name. It will warn for each missing file.
</div>
