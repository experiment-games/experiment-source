---
template: lua-class-function.html
title: GetFriendCount
icon: lua-shared
tags:
  - lua
  - shared
  - needs-verification
  - needs-example
lua:
  library: SteamFriendsHandle
  function: GetFriendCount
  realm: shared
  description: "Get the number of friends the current user has of the specified flag types."
  
  arguments:
  - position: 1
    name: "steamFriends"
    type: SteamFriendsHandle
  - position: 2
    name: "flag"
    type: number
  returns:
    - type: integer
      description: "The number of friends the current user has."
---

<div class="lua__search__keywords">
SteamFriendsHandle:GetFriendCount &#x2013; Get the number of friends the current user has of the specified flag types.
</div>
