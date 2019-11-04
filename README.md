# OpenGameCamera

OpenGameCamera is an open source cinematic camera modification for Battlefront II.

### Discord Server

For communication, support, sharing, and development, join our server [here](https://discord.gg/HZ676Ff)

## Downloads

Download the latest release from [Here](https://github.com/coltonon/OpenGameCamera/releases).  Pre-release builds only include a DLL, which must be injected manually.

# How To Use
You'll need a DLL injector to load it into the game, you can download my old one [here](https://cdn.discordapp.com/attachments/548738758053330944/640624916323893297/Injector.exe).

**Install [this](https://aka.ms/vs/16/release/vc_redist.x64.exe)** (C++ redistributable)

## Key Bindings

| Action | Key |
| --- | --- |
| **Enter/Exit camera** | F5 |
| **Ignore input** | F6 |
| **Show/Hide UI** | F7 |
| **Time-freeze** | F8 |
| **Move camera** | WASD |
| **Cam up** | Space |
| **Cam down** | Ctrl |
| **Fast move** | Shift |
| **Slow move** | Alt |
| **Hide OpenGameCamera** | Home |

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