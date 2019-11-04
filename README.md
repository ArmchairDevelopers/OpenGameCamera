# OpenGameCamera

OpenGameCamera is an open source cinematic camera modification for Battlefront II.

### Discord Server

For communication, support, sharing, and development, join our server [here](https://discord.gg/HZ676Ff)

## Code Structure

**Source.cpp**: The entrypoint of the modification, and where all of the magic happens.  

**MouseManager.hpp**: Management class for hooking the mouse state, allowing the mod to enable/disable the cursor at will.

**KeyMan.cpp/hpp**: HotKey management.  Also defined here are the default keybindings.

**Renderer.hpp**: Code for hooking into the engine's DebugRenderer functions.

**Menu.hpp**: Using renderer.hpp, this code allows easily building and displaying menus.

**Candy.hpp**: Hooking class.  Allows VTable hooking, and a wrapper for MinHook.

**Typedefs.hpp**: Contains typedefs used for our hooks.

**sdk.hpp**: The reverse-engineered game classes.

**BasicTypes.hpp**: Definitions for things like vectors and matrices.  Serves to replate DirectX.