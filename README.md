![ogc-banner](https://opengamecameralauncher.fra1.digitaloceanspaces.com/launcher-infomercial.png)

# What is OpenGameCamera?

OpenGameCamera is a free, open-source camera tool for Frostbite games. Our aim is to build a robust toolset with the freedom to allow content creators, modders and PC users to create stunning artwork and movies.

## Instructions

### [Download the latest version of OpenGameCamera](https://github.com/coltonon/OpenGameCamera/releases)

1. Download the OpenGameCamera Launcher.

2. Run the Launcher and click 'Launch OpenGameCamera'.

3. Make awesome content.

### Discord Server

For communication, support, sharing, and development, join our Discord server: [OpenGameCamera Discord](https://discord.gg/HZ676Ff)  

### Join the team!

We are currently expanding the core dev team, so if you know a thing or two about reverse engineering game camera systems, reach out! We're most active on [the official OpenGameCamera Discord](https://discord.gg/HZ676Ff)

## Key Bindings

| Action | Key |
| --- | --- |
| **Show/Hide OpenGameCamera** | Home |
| **Enter/Exit camera** | F5 |
| **Ignore input** | F6 |
| **Show/Hide UI** | F7 |
| **Freeze Time** | F8 |
| **Toggle Depth of Field** | F9 |
| **Move camera** | WASD |
| **Cam up** | Space |
| **Cam down** | Ctrl |
| **Fast move** | Shift |
| **Slow move** | Alt |
| **Toggle Resolution Scale** | Insert |
| **Eject mod** | End |

*Hint: You can Ctrl+Click on a slider to type in a value!*

# Building

## Code Structure

| File | Purpose |
| --- | --- |
| **Source.cpp** | The entrypoint of the modification, and where all of the magic happens |
| **MouseManager.hpp** | Management class for hooking the mouse state, allowing the mod to enable/disable the cursor at will |
| **KeyMan.cpp/hpp** | HotKey management.  Also defined here are the default keybindings. |
| **Renderer.hpp** | Code for hooking into the engine's DebugRenderer functions. |
| **Menu.hpp** | Using renderer.hpp, this code allows easily building and displaying menus. |
| **Candy.hpp** | Hooking class.  Allows VTable hooking, and a wrapper for MinHook. |
| **Typedefs.hpp** | Contains typedefs used for our hooks. |
| **sdk.hpp** | The reverse-engineered game classes. |
| **BasicTypes.hpp** | Definitions for things like vectors and matrices.  Serves to replate DirectX. |

## Building guide

The code is designed to be easily built.  You don't even need the DirectX SDK installed.

1. Clone, or [download](https://github.com/coltonon/OpenGameCamera/archive/master.zip) and extract
2. Open `OpenGameCamera` in Visual Studio
3. Build (Ctrl-B)

## Credits

[Coltonon](https://github.com/coltonon)  
[BattleDash](https://github.com/BattleDash)  
[Dangercat](https://github.com/Dangercato)  
[cstdr](https://github.com/cstdr1)  
[GalaxyMan](https://github.com/GalaxyEham)  
[Dulana57](https://github.com/Dulana57)  
[kiwidog](https://github.com/kiwidoggie)  


# Docs

[Here](https://github.com/coltonon/OpenGameCamera/tree/master/Docs) you can find extended documentation, as well as explantions on various concepts.
