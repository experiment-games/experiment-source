---
template: lua-type.html
title: number
tags:
  - lua
  - needs-verification
lua:
  type: number
  isDefault: true
---

The `number` type is a data type that represents a number.
Lua uses double-precision (64-bit) floating-point numbers, which means that the
maximum value for a number is approximately 1.8<sup>308</sup> and the minimum
value is approximately 2.2<sup>-308</sup>.

Numbers can be positive, negative, or zero. They can also be integers or
floating-point numbers.

## Integer numbers

Since Lua 5.3 checks for integers will fail if the number can not be represented
as an integer. This means that a number like `100.33333333333333` will fail.
Since we use Lua 5.4 this is relevant for us.

Because of this, internally we always check for a number, even if we need an
integer. This means that any decimal portion is truncated (lost, not rounded).

In the documentation you will see where integers are expected. This is to make
it clear that the value you pass will be truncated to an integer.

This is useful in situations where you want to do some math to compute a value.
For example when positioning a panel:

  ```lua
  local x = Utilities.GetScreenWidth() / 3

  panel:SetPosition(x, 50)
  ```

If `x` ends up being something like `100.33333333333333`, the panel would
error with `number has no integer representation` if we checked for an integer.

However, since we cast the value to an integer, it will be truncated to `100`.
So note that `panel:GetPosition()` will return `100` for the x value and not
`100.33333333333333`.
