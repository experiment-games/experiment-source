# Hooks

Using the [Hooks Library](../../libraries/Hooks/index.md) you can attach code to certain events in the game. This allows you to run custom code when certain things happen in the game.

A function registered to a hook will execute when that event occurs. The typical pattern involves:

```lua
Hooks.Add("Think", "MyScriptUpdate", function()
    -- Custom logic here
end)
```

This ensures that custom logic runs at the appropriate time.

You can find all the hooks in [Hooks](../../hooks/index.md) section of the documentation.
