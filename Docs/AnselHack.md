# Disabling Ansel Distance Limits

In this quick explanation, we're going to learn how you can change the distance limitation set by ansel, that controls how far you can move the camera from the player.
Starting off, we're going to need three different tools.
* Typeinfo.studio [https://typeinfo.studio/](https://typeinfo.studio/)
* Reclass.NET
* Cheat Engine
Typeinfo.studio is a fancy Runtime Type Information (shorthanded as TypeInfo) dumping webapp
First thing we'll do is load the most recent dump, the one I just took.

![](https://i.imgur.com/Pn3WPfv.png)

Then we'll search for AnselSettings

![](https://i.imgur.com/QdIxvUv.png)

If you click on the Node View to view members, you'll find this float at 0x90 

![](https://i.imgur.com/MrMJQU2.png)

Now basically what we must do, is find all instances of AnselSettings, find the instance that is authoritative (meaning the changes you write to memory will actually do something), then of course, change our setting
The way we'll find this will be through the vtable.  A VTable is a table of functions present in a class.  At the beginning of a class, at 0x0, you will find a pointer to the vtable.  This means if we know the current vtable address, we can simply search it with CheatEngine, and we'll get the exact addresses of all instances of that class.  Since the dump on TypeInfo.Studio was taken today, this will give us the VTable address for the current game patch.
Simply copy the vtable address

![](https://i.imgur.com/L0LCJ06.png)

Now open Cheat Engine, and search for that address.  Note you must search for an 8 byte value, since that is the size of a pointer on a 64 bit application.  Also you must search for hexidecimal values.
![](https://i.imgur.com/JfaiptV.png)

You should have two values found.  One is the default instance, which is not authoritative.  The other value is the one we're looking for. 
![](https://i.imgur.com/LgdRqhb.png)

If we fire up Reclass and input the address of the Default Instance from Typeinfo.studio, we'll see the default instance is the second scan result from Cheat Engine. 
![](https://i.imgur.com/WjkGDOX.png)

This means the 2e1c61b0 address is what we need.  Note that this is HEAP memory, thus this address will be different on every machine, every time you start the game.
Now, if we enter that address into Reclass, and expand the class, then reference the screenshot from Typeinfo.studio's node view, we know that at 0x2e1c61b0 + 0x90 we should have a float that determines how far the camera can wander. 
![](https://i.imgur.com/P4jLBdk.png)

Now we set this type as a float, then change it to a really large number.
![](https://i.imgur.com/XFcG3Fa.png)

Now if we enable Ansel in our game, we should be able to move the camera an unreasonably long distance
![](https://i.imgur.com/eG3jKRU.jpg)

Sure enough, we can.
Now, to make this easier to do and not require cheat engine, lets find a static offset.
Since AnselSettings is of type SystemSettings, it's likely to have a static pointer directly to the authoritative class.  By simply searching our instance address in Cheat Engine, we should find a result.
![](https://i.imgur.com/Zqt5NVy.png)
![](https://i.imgur.com/OKMTUh2.png)

Sure enough, the green address is what we were looking for
I'm going to define this as OFFSET_ANSELSETTINGS: 0x14436ea28
If you enter this into reclass on the current build of this game, you'll find a pointer to the authoritative AnselSettings
Set up your Reclass Project like this: 
![](https://i.imgur.com/IkmswUx.png)

Now, what happens when the game updates?  These offsets will change, so we should probably make a signature.
Rather than dumping the game and loading it in ida to make a sig, we'll do this with CE's debugger real quickly.
![](https://i.imgur.com/XE1LOGY.png)

Once you've attached CE's debugger, enter Ansel's camera, and you should see some addresses of code that've touched this address. 
![](https://i.imgur.com/hG47DMH.png)

We'll look at one of these addresses in the disassembler, and manually create a signature.
![](https://i.imgur.com/9h9umxH.png)

the bytes I've crossed out have to be wildcarded, since they'll change on different builds of the game.
This gives us a complete sig of 
```48 8b 05 ?? ?? ?? ?? 8b 88 88 00 00 00 89 4b 10 f3 0f 11 82 58 01 00 00```
Now, lets go through how to do this at a later update:
Search for the above byte array with these exact settings: 
![](https://i.imgur.com/M8QeLWR.png)

You should have one scan result.
![](https://i.imgur.com/1zOs5VT.png)

Open that address in the disassembler
![](https://i.imgur.com/bVK5HGC.png)

The address inside of those brackets is our static offset we need. It'll be valid until the game updates.
Then open Reclass, paste that address, dereference at 0, then create a float at 0x90. 
![](https://i.imgur.com/d4oOrTY.png)

Change that float to whatever value you want, then you're good to go!
Note this value is read only when you enter the ansel camera, so if you're already in ansel mode, simply toggle out, then toggle back in.
Just tag me in #general if you need any help with doing this.
Also this link takes you directly to the typeinfo dump of AnselSettings [https://typeinfo.studio/?file=/dumps/starwarsbattlefrontii-06-03-2019.json&typeInfo=AnselSettings&node=true](https://typeinfo.studio/?file=/dumps/starwarsbattlefrontii-06-03-2019.json&typeInfo=AnselSettings&node=true)

Okay now to write a dll that'll do all of this for us
I'm gonna create a new empy C++ project in visual studio, set the build mode to release, 64 bit, and compile as a dll
Then I'll add all of the sigscanning code from bfcamera, found here [github.com/coltonon/BfCamera/tree/master/BfCamera/SigScan](https://github.com/coltonon/BfCamera/tree/master/BfCamera/SigScan)

![](https://i.imgur.com/hzmgryw.png)

set it up like this in your project
Except the contents of StaticOffsets.h should be:

```cpp
#include "SigScan.h"
#define AddOffset(name, pattern, type) static DWORD64 Get_## name (){static DWORD64 name = NULL; if (name != NULL) return name; return name=(DWORD64)PatternScanner::FindPattern({pattern, type});} 

class StaticOffsets {
public:
    AddOffset(OFFSET_ANSELSETTINGS, "48 8B 05 [?? ?? ?? ?? 8B 88 88 00 00 00 89 4B 10 F3 0F 11 82 58 01 00 00", PatternType::RelativePointer)
};
```
Then we'll create a .cpp file as the entrypoint of the mod
name it whatever the hell you want
At the top of it, we'll include our sigscanning class, and define a bool method that'll check if a pointer is valid or not
```cpp
#include "StaticOffsets.h"

// check if a pointer is valid
bool IsValidPtr(void* ptr) {
    return !IsBadReadPtr(ptr, sizeof(LPVOID));
}
then we'll have our reversed game class 
// The game's AnselSettings class
class AnselSettings {
public:
    char pad_000[0x90];
    float CameraMaxWanderDistance;
};
```
Then our main thread of the module, it's commented what it does
```cpp
DWORD WINAPI mainThread(LPVOID lpParam) {
    // Get the address from our sig scanner
    void* ppAnselSettings = (void*)StaticOffsets::Get_OFFSET_ANSELSETTINGS(); 
    // Wait to make sure the pointer is valid
    while (!IsValidPtr(ppAnselSettings)) Sleep(100);
    // Dereference
    AnselSettings* pAnselSettings = *(AnselSettings**)ppAnselSettings;
    // set the MaxWanderDistance to some other number
    pAnselSettings->CameraMaxWanderDistance = 10000000;
    // unload the module
    FreeLibraryAndExitThread((HMODULE)lpParam, 0);
}
```
Finally create an entrypoint of the mod
```cpp
// entry point of the mod
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    // if the mod just got attached to the game,
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // create a thread and start executing our own code
        CreateThread(NULL, NULL, mainThread, hModule, NULL, NULL);
    }
    return TRUE;
}
```
Then build it, inject it, and you're good
optionally toss it in your cmods folder and edit the json file so it loads natively with the game, that way it just becomes part of the game
For those of you who don't wanna go through building this, just inject this dll: [link](https://cdn.discordapp.com/attachments/552406643543900161/585136542712987650/AnselHack.dll)
If you have the BattlefrontStats tracker installed (you all should for crying out loud), you can just do this:
paste this as your modlist.json file:
```json
["cmods/stats-tracker/stats-tracker.dll", "cmods/AnselHack/AnselHack.dll"]
```
Then we have to create a directory in the cmods folder called AnselHack, and place AnselHack.dll in it
![](https://i.imgur.com/RwpU5VR.png)

Like so

Sure enough, soon as we start up the game this is set
https://i.imgur.com/ayzaOXS.png

Just like bfstats, there's no special startup or external tools required, if you install this mod to your cmod directory like I showed you should be good forever and not have to worry about ansel distance limitation ever again
so, TLDR:
Install bfstats mod if you haven't that way you have the cmods stuff installed
edit cmods/modlist.json so it looks like: 
```json
["cmods/stats-tracker/stats-tracker.dll", "cmods/AnselHack/AnselHack.dll"]
```

Create a folder in the cmods folder named AnselHack
Download that AnselHack.dll file and place it in the AnselHack folder
Start the game as normal

# Update

AnselHack has it's own repo, view [here](https://github.com/coltonon/anselhack)