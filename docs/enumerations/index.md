---
template: lua-index.html
childrenTitle: Enumeration Types
include:
  startsWith: enumerations/
  isIndex: true
exclude:
  exact: enumerations/
format:
  label: '%parentTitle%'
  hideParent: true
---

# Enumerations (`_E`)

An enumeration (or enum) is a set of named integer constants. That is to say, a
list of names that are associated with numbers. Enumerations are used instead of
just numbers ("magic numbers") to make the code more readable and maintainable.

Internal enumerations are stored in the global `_E` table. Each enumeration is
grouped under its own sub-table.

## Your own enumerations

You could (and should) implement enumerations yourself when you have a set of
constants that are related to each other. For example, if you have a set of
roles in your game, you could define an enumeration for them:

=== "Using Enumerations (recommended)"

    In this example `ROLES.ADMIN` is an enumeration that represents the admin role.
    It's immediately clear what role is checked for, without having to look at the
    rest of the code.

    ```lua hl_lines="1-4 6"
    ROLES = {
      GUEST = 0,
      ADMIN = 1,
    }

    if (client:GetRole() == ROLES.ADMIN) then
        client:DoRestrictedThing()
    end
    ```

=== "Magic Numbers (bad practice)"

    In this example `1` is a "magic number" that represents the admin role. This
    is bad because it's not clear what `1` means without diving into what the
    `client:DoRestrictedThing()` function does, and then trying to figure out
    which role you think that function should be restricted to.

    ```lua hl_lines="1"
    if (client:GetRole() == 1) then
        client:DoRestrictedThing()
    end
    ```
