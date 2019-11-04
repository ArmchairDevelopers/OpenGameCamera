# Camera Transform

So the camera matrix is a 4x4 array of floats, that describes the current camera location, pitch, yaw, and roll.  But it's not expressed in Euler angles, like you're probably used to seeing instead, it's formatted where the first row is a relative coordinate to where the camera is on the X axis in the game at +1 meters.  The second row is the same for Y, third is the same for Z, and the fourth row is the absolute location for the camera in the world-space.  So when it comes to writing these locations based on keyboard input, there's virtually no math involved.

![](https://i.imgur.com/mvpsJW8.png)

All we have to do is take any given row, and multiply it by our movement modifier (so instead of 1 meter, change it to however many meters you want to move at once), then add it to our absolute camera location in game as a visual, ![](https://i.imgur.com/XlS6C7o.png)

The second row is the height axis (y axis in world coordinates)
since it shows the relative position of +1 meters in it's axis, this tells us that the camera is pretty much vertical.  The x value is perfectly aligned, the y value isn't quite 1.0, so we're leaned towards the z axis a little bit, as shown in the z value of that vector hopefully this helps you understand how these camera matrixes work in game, and why it makes sense to use a system like this rather than a vector of it's location in combination of pitch/yaw/roll values.