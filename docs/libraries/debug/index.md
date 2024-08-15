---
template: lua-index.html
childrenTitle: Functions
include:
  startsWith: libraries/debug/
exclude:
  isIndex: true
---

# Debug Library

The `debug` library is a standard Lua library that provides a variety of functions
for debugging and introspection. We've extended the `debug` library with additional
functions to help you debug your code.

Original functions have non-capitalized names, e.g: `debug.getinfo`, while our
additional functions have UpperCamelCase names, e.g: `debug.PrintWarning`.

Read more on the standard Lua `debug` library [in the Lua PIL](https://www.lua.org/pil/23.html)
or [in the Lua 5.4 Reference Manual](https://www.lua.org/manual/5.4/manual.html#6.10).
