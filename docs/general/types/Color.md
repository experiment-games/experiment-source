---
template: lua-type.html
title: Color
tags:
  - lua
  - needs-verification
lua:
  type: Color
---

The `Color` type represents a color. Colors are used to represent RGBA values.

Colors have four components: `red`, `green`, `blue`, and `alpha`.
These components can be accessed using the dot operator or the index operator:

```lua
local color = Color(255, 200, 100, 55)

print(color.red == color[1] and color.red == 255) -- true
print(color.green == color[2] and color.green == 200) -- true
print(color.blue == color[3] and color.blue == 100) -- true
print(color.alpha == color[4] and color.alpha == 55) -- true
```

Colors can be created using the [`Colors.Create`](libraries/Colors/Create.md)
function. The [`Colors` library](libraries/Colors/index.md) provides functions to
help create colors to and from different formats.
