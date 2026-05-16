# A Game Engine for the PlayStation 2

Yet another 3D game engine. But this one runs on the PS2!

<img src="https://media.discordapp.net/attachments/684797216229294101/1333280276780023829/image.png?ex=67985185&is=67970005&hm=7c90b25d22b819f9b06ce398f31f6a7e88fb0f255ed74be779333906c95e207b&=&format=webp&quality=lossless" width=400>

| Essential Features |  |
|-------------------------------------------------------------|--|
| Basic 3D Rendering | ✓ |
| Model Loading | ✓ |
| Textures | ✓ |
| Character Animation | |
| Controller Input | ✓ |
| Text Rendering | |
| GUI | |
| Physics | |
| Collision Detection | |
| Game AI | |
| Audio | |
| Video Player | |

Build Instructions
---
1. Setup [ps2sdk](https://github.com/ps2dev/ps2dev) on your local machine, or use [their docker container](https://hub.docker.com/r/ps2dev/ps2dev).
2. Configure CMake:
```
cmake -S. -Bbuild -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake
```
3. Build to create the ELF:
```
cmake --build build
```
