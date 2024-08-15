---
template: lua-type.html
title: Vector
tags:
  - lua
  - needs-verification
lua:
  type: Vector
---

The `Vector` type represents a three-dimensional vector. Vectors are used to
represent positions, directions, and velocities in 3D space.

Vectors have three components: `x`, `y`, and `z`. These components can be
accessed using the dot operator:

```lua
local position = Vector(1, 2, 3)
print(position.x) -- 1
print(position.y) -- 2
print(position.z) -- 3
```

They can also be accessed using the index operator:

```lua
local position = Vector(1, 2, 3)

print(position[1]) -- 1
print(position[2]) -- 2
print(position[3]) -- 3
```

Vectors can be created using the [`Vectors.Create`](../../libraries/Vectors/Create.md)
function.
