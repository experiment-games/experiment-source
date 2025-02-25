---
template: lua-index.html
childrenTitle: Methods
include:
  startsWith: classes/Angle/
exclude:
  isIndex: true
---

# Angle

The `Angle` class represents a three-dimensional angle. Angles are used to
Euler angles in 3D space.

Angles have three components: `pitch`, `yaw`, and `roll`. These components can
be accessed using the dot operator:

```lua
local orientation = Angle(0, 90, 45)

print(orientation.pitch) -- 0
print(orientation.yaw) -- 90
print(orientation.roll) -- 45
```

They can also be accessed using the index operator:

```lua
local orientation = Angle(0, 90, 45)

print(orientation[1]) -- 0
print(orientation[2]) -- 90
print(orientation[3]) -- 45
```

Angles can be created using the [`Angles.Create`](../../libraries/Angles/Create.md)
function.
