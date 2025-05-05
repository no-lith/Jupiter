

[TOC]



# Jupiter

Port of PC Enterprise Edition Build 69 to C++20.

This port doesn't include any game code, just the engine, server, sound, clientfx, libraries and samples.

The tools have not been ported, PR is welcome.

# Requirements
[Visual Studio 2022](https://visualstudio.microsoft.com/vs/)

[Premake](https://premake.github.io)

# How to compile?

> git clone https://github.com/no-lith/Jupiter.git
>
> copy premake5.exe to the root folder
>
> run cmd in the root folder with: premake5 vs2022

This will generate all the solutions and projects

**Jupiter-Engine**: Core, ClientFX, , Render, Server and Sound
**Jupiter-Libs**: Shared libs
**Samples**: Each sample has its own solution, usually the projects of a sample are CRes (Client Res), CShell (ClientShell), SShell or Object (ServerShell) and SRes (Server Res).

# TODO

- [ ] Port tools
- [ ] Address sanitizer
- [ ] Fix all warnings

# Overview

```
📦Jupiter
 ┣ 📂Engine
 ┃ ┣ 📂ClientFX
 ┃ ┣ 📂LIB-Info
 ┃ ┣ 📂LIB-Render
 ┃ ┣ 📂LIB-Sound
 ┃ ┣ 📂LIB-UI
 ┃ ┣ 📂LithTech
 ┃ ┣ 📂Resources
 ┃ ┣ 📂runtime
 ┃ ┃ ┣ 📂client
 ┃ ┃ ┣ 📂controlfilemgr
 ┃ ┃ ┣ 📂esd
 ┃ ┃ ┃ ┣ 📂RealFileSystem
 ┃ ┃ ┃ ┣ 📂RealFileSystemInstaller
 ┃ ┃ ┣ 📂info
 ┃ ┃ ┣ 📂kernel
 ┃ ┃ ┃ ┣ 📂io
 ┃ ┃ ┃ ┣ 📂mem
 ┃ ┃ ┃ ┣ 📂net
 ┃ ┃ ┃ ┗ 📂toport
 ┃ ┃ ┣ 📂lithtemplate
 ┃ ┃ ┣ 📂model
 ┃ ┃ ┣ 📂physics
 ┃ ┃ ┣ 📂render_a
 ┃ ┃ ┣ 📂render_b
 ┃ ┃ ┣ 📂server
 ┃ ┃ ┣ 📂shared
 ┃ ┃ ┣ 📂sound
 ┃ ┃ ┣ 📂ui
 ┃ ┃ ┗ 📂world
 ┃ ┣ 📂sdk
 ┃ ┃ ┣ 📂inc
 ┃ ┃ ┃ ┣ 📂esd
 ┃ ┃ ┃ ┣ 📂libltdiagnostics
 ┃ ┃ ┃ ┣ 📂libltinfo
 ┃ ┃ ┃ ┣ 📂physics
 ┃ ┃ ┃ ┣ 📂state_mgr
 ┃ ┃ ┃ ┣ 📂sys
 ┃ ┃ ┣ 📂rez
 ┃ ┃ ┃ ┣ 📂RenderStyles
 ┃ ┃ ┃ ┃ ┣ 📂Extras
 ┃ ┃ ┃ ┃ ┣ 📂Textures
 ┃ ┣ 📂Server
 ┃ ┣ 📂Sound
 ┣ 📂Libs
 ┃ ┣ 📂LIB-ButeMgr
 ┃ ┣ 📂LIB-CryptMgr
 ┃ ┣ 📂LIB-DIBMgr
 ┃ ┣ 📂LIB-DShow
 ┃ ┣ 📂LIB-DTXMgr
 ┃ ┣ 📂LIB-GenRegMgr
 ┃ ┣ 📂LIB-GuiMgr
 ┃ ┣ 📂LIB-Lith
 ┃ ┣ 📂LIB-LTAMgr
 ┃ ┣ 📂LIB-LTMem
 ┃ ┣ 📂LIB-MFCStub
 ┃ ┣ 📂LIB-RegMgr
 ┃ ┣ 📂LIB-RegMgr32
 ┃ ┣ 📂LIB-RezMgr
 ┃ ┣ 📂LIB-StackTracer
 ┃ ┣ 📂LIB-STDLith
 ┃ ┣ 📂LIB-ZLib
 ┣ 📂Samples
 ┃ ┣ 📂audio
 ┃ ┃ ┣ 📂music
 ┃ ┃ ┗ 📂sounds
 ┃ ┣ 📂base
 ┃ ┃ ┣ 📂mpflycam
 ┃ ┃ ┣ 📂samplebase
 ┃ ┃ ┗ 📂simplephys
 ┃ ┣ 📂content
 ┃ ┃ ┗ 📂Maya
 ┃ ┃ ┃ ┣ 📂SealHunter
 ┃ ┃ ┃ ┃ ┣ 📂Textures
 ┃ ┣ 📂debugging
 ┃ ┃ ┗ 📂stacktrace
 ┃ ┣ 📂graphics
 ┃ ┃ ┣ 📂bump
 ┃ ┃ ┣ 📂clientfx
 ┃ ┃ ┣ 📂drawprim
 ┃ ┃ ┣ 📂effects
 ┃ ┃ ┣ 📂fonts
 ┃ ┃ ┣ 📂GuiMgr
 ┃ ┃ ┣ 📂renderdemo
 ┃ ┃ ┣ 📂shaders
 ┃ ┃ ┣ 📂specialeffects
 ┃ ┃ ┗ 📂video
 ┃ ┣ 📂models
 ┃ ┃ ┣ 📂animations
 ┃ ┃ ┣ 📂animations2
 ┃ ┃ ┣ 📂attachments
 ┃ ┃ ┗ 📂obb
 ┃ ┣ 📂networking
 ┃ ┃ ┣ 📂nettest
 ┃ ┃ ┗ 📂sealhunter
 ┃ ┣ 📂objects
 ┃ ┃ ┣ 📂doors
 ┃ ┃ ┣ 📂pickups
 ┃ ┃ ┗ 📂projectiles
 ┣ 📂Tools
 ┃ ┗ 📂shared
 ┃ ┃ ┣ 📂engine
 ┃ ┃ ┣ 📂model
 ┃ ┃ ┣ 📂packer
 ┃ ┃ ┗ 📂world
 ┗ 📂Vendor
 ┃ ┣ 📂directx-8.1
 ┃ ┣ 📂eax
 ┗ ┗ 📂s3tc
```


# License

The engine code is from the apparent GPL version of the Jupiter Ex engine before Touchdown Entertainment disappeared. The GPL version was PC Enterprise Edition Build 69. This build was released in 2006, GPLv3 was released in 2007.

Touchdown Entertainment, Inc. which was a subsidiary of Monolith Productions, Inc. Monolith Productions, Inc. All rights reserved.

All other trademarks are copyright by their respective owners.