Music

This sample demonstrates the music functionality of Jupiter.  Since
Jupiter's music implementation is DirectMusic based, segment files 
(*.sgt) are used for describing music resources.  For more information
on segment files, refer to the DirectX SDK as well as DirectMusic 
Producer (used for creating/modifying DirectMusic resources).

A control file (ex. music.txt in the \rez\Music folder) is used to 
organize segments and describe what segments should be played.  The 
control file is also used for setting up dynamic music.  Intensities 
are used to list segments to be played.  For more information on 
setting up a control file, take a look at the control file used by 
this sample.

Moving the camera into each of the four platforms causes an intensity 
to be played.  Intensities can also be controlled using the keyboard.


Key Configuration:

Up Arrow	- Apply Intensity
Down Arrow	- Stop
Left Arrow	- Previous Intensity
Right Arrow	- Next Intensity
Space		- Pause
I		- Initial play

W		- Forwards
S		- Backwards
A		- Left
D		- Right

Escape		- Quit Sample


Questions to: support@touchdownentertainment.com