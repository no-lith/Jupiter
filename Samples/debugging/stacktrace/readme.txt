StackTrace

This sample is based off of the SampleBase sample. The purpose of this sample
is to show the usage of the Lib_StackTracer library.

You must setup a few things before stack tracing is possible.
1. You must build everything in Debug. (This includes the engine)
2. You must set the autoexec.cfg variables SymbolPath0, SymbolPath1, SymbolPathN... 
   to directories containing exes, dlls, and PDBs for them.
3. You must explicitly set the base address for cshell.dll. 
   (the project is currently set to 0x62000000)
4. dbghelp.dll must be in your bin directory. (which it should be)

Once these are set up, simply double click "run_debug.bat" and hit the "T" key. The application
may pause for a few seconds while it loads the PDB symbols. You should then get a call stack 
listing in the Jupiter console.


Please use the following commands to move the camera:

Key Configuration:

Up Arrow	- Forwards
Down Arrow	- Backwards
Left Arrow	- Left
Right Arrow	- Right

W		- Forwards
S		- Backwards
A		- Left
D		- Right

T		- Create a StackTrace and print to the console
~		- Access the console

Escape		- Quit Sample


Questions to: support@touchdownentertainment.com
