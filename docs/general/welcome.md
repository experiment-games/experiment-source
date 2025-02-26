# Welcome to the Experiment Developer Portal

This wiki documents the [Lua bindings](../general/lua-bindings.md) to the [Source Engine (Source SDK 2013)](https://developer.valvesoftware.com/wiki/Source_SDK_2013) in Experiment.
With this wiki we aim to provide reference material for the Lua API, as well as tutorials and examples to help you get started with Lua scripting in Experiment.

With Experiment we aim to help you quickly prototype Source Engine mods without having to write any C++ code.

!!! warning

    Experiment is still in development and the Lua API is subject to change. Right now is not
    the best time to use it for a large project. Instead, we recommend using it for playing around
    with the API and providing feedback to us.

## First Steps

Depending on your experience we recommend the following steps to get familiar with the Experiment Lua API:

??? note "For Beginners &nbsp; |&nbsp; ⌚ 1-2 hours"

    Experience Required: **Minimal scripting experience**

    1. **Follow [The Basics (tutorial)](./the-basics.md)**

        This will guide you through the basics of Lua scripting in Experiment by creating:

        - A simple script that launches all players into the air on command.
        - Converting that script into a reusable function.
        - Adding parameters to the function and command to control the launch height.
        - Converting the script into an addon that can be shared with others.

    2. **Read up on the [Concepts](./concepts/index.md) in the Experiment Lua API**

        Learn how hooks, classes, libraries and more can help you create more complex mods.
        It contains some examples that can help you understand how to use these concepts.

    3. **Save yourself and your fellow developers time by reading the [Best Practices](./best-practices.md)**

        Learn how to write clean, efficient and maintainable Lua code.
        Additionally this shows you how to deduce which functions can help you achieve your goals.

??? note "For Intermediate Users &nbsp; |&nbsp; ⌚ 1 hour"

    Experience Required: **Basic Lua scripting experience**

    1. **Read up on the [Concepts](./concepts/index.md) in the Experiment Lua API**

        Learn how hooks, classes, libraries and more can help you create more complex mods.
        It contains some examples that can help you understand how to use these concepts.

    2. **Save yourself and your fellow developers time by reading the [Best Practices](./best-practices.md)**

        Learn how to write clean, efficient and maintainable Lua code.
        Additionally this shows you how to deduce which functions can help you achieve your goals.

    3. **Head over to the API References:**

        - [Hooks](../hooks/index.md)
        - [Classes](../classes/index.md)
        - [Libraries](../libraries/index.md)
        - [Enumerations](../enumerations/index.md)
        - [Structures](../structures/index.md)
