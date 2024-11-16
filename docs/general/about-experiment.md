# About Experiment

Experiment is a Source Engine mod that adds Lua scripting support to the [Source Engine (Source SDK 2013)](https://developer.valvesoftware.com/wiki/Source_SDK_2013).
With Experiment you can write mods for Source Engine games using the [Lua (5.4) scripting language](https://www.lua.org/).

We aim to help you quickly prototype Source Engine mods without having to write any C++ code.
This way you can focus on the gameplay and content of your mod, without having to worry about the technical details of the Source Engine.

Despite us saying 'prototype', you can create fully-fledged mods with Experiment.
When you're ready to release your mod, we'll provide the tools and Experiment source-code so you can compile your mod into a standalone game.

## Mounting Content

The Source Engine has a great feature called 'mounting' that allows you to load content from other Source Engine games.
Within the Experiment menu you can select which games you want to mount content from.
All models, textures, sounds, etc. from the mounted games will be available to your mod.

## Compatibility with Garry's Mod

In order to kickstart your adventure with Experiment, we want to make sure you can use as much of your existing code as possible.
This is why we aim to make Experiment as compatible as possible with [Garry's Mod](https://gmod.facepunch.com/) Lua scripts.

!!! Note
    We are not affiliated with Garry's Mod or [Facepunch Studios](https://facepunch.com/)

Read more on compatibility in the [gmod_compatibility](../modules/gmod_compatibility.md) module documentation.

## Origins

We started Experiment with the goal of porting our Garry's Mod gamemode [Experiment Redux](https://github.com/luttje/gmod-experiment-redux) to the Source Engine.
Not wanting to rewrite all our Lua code in C++, we decided to add Lua scripting support to the Source Engine instead.
Because of this, we ended up basically having to implement everything that our gamemode uses in Garry's Mod, but in the Source Engine instead.

A great source of help and inspiration for us was the [Half-Life 2: Sandbox](https://github.com/Planimeter/hl2sb-src) project by [Planimeter](https://www.planimeter.org/).
It allowed us to get a head start in implementing Lua scripting support in the Source Engine.

With code from that great project we continued work on getting the Experiment Redux gamemode to work in our Source Engine mod.
Our first big milestone was getting the [Garry's Mod sandbox gamemode](https://github.com/Facepunch/garrysmod/tree/54933ee83bcb52cbf7436d380a079e9f10d161fb/garrysmod/gamemodes/sandbox) to load and run in our mod.

From there we continued to add more and more features to our mod, and we are still working on it to this day.
To see what our goals and roadmap are, check out the [Goals and Roadmap](../general/goals-and-roadmap.md) page.
