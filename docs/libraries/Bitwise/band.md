---
template: lua-library-function.html
title: band
lua:
  library: Bitwise
  function: band
  realm: shared
  arguments:
    - name: a
      type: number
      description: The first number.
    - name: b
      type: number
      description: The second number.
  returns:
    - type: number
      description: The result of the bitwise AND operation.
  description: Performs a bitwise AND operation on two numbers.
  remarks:
    - type: note
      content: The `band` function is equivalent to the `&` operator in Lua.
  related:
    - libraries/bitwise/bor
---

Checks if the first player has the `FL.ONGROUND` flag set:

=== "Bitwise.band"

    ```lua hl_lines="2"
    local player = Util.GetPlayerByUserID(1)
    local isOnGround = Bitwise.band(player:GetFlags(), FL.ONGROUND) ~= 0

    if (isOnGround) then
      print("Player 1 is on the ground.")
    end
    ```

=== "& operator"

    ```lua hl_lines="2"
    local player = Util.GetPlayerByUserID(1)
    local isOnGround = player:GetFlags() & FL.ONGROUND ~= 0

    if (isOnGround) then
      print("Player 1 is on the ground.")
    end
    ```

Checks if the player has *only* he `FL.ONGROUND` flag set:

=== "Bitwise.band"

    ```lua hl_lines="2"
    local player = Util.GetPlayerByUserID(1)
    local isOnGroundAndNothingElse = Bitwise.band(player:GetFlags(), FL.ONGROUND) == FL.ONGROUND

    if (isOnGroundAndNothingElse) then
      print("Player 1 is on the ground and no other flags are set.")
    end
    ```

=== "& operator"

    ```lua hl_lines="2"
    local player = Util.GetPlayerByUserID(1)
    local isOnGroundAndNothingElse = player:GetFlags() & FL.ONGROUND == FL.ONGROUND

    if (isOnGroundAndNothingElse) then
      print("Player 1 is on the ground and no other flags are set.")
    end
    ```
