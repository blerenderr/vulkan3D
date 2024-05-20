# vulkan3D Concept Engine
Cool name coming soon.

![engineicon](https://github.com/blerenderr/vulkan3D/assets/71245490/8833f8b7-5f3f-4941-ba42-754b5830e0f3)
*(official logo)*


## Why?
I started this as a successor to the software renderer for a few reasons.
  1. Rasterizers and textures are fucking hard and I just want to let Vulkan handle it
  2. The software engine's code (don't look at it) is horrible because I didn't really know C or C++
  3. I get a fresh start using my improved knowledge of C
  4. Utilizing the GPU will let me explore lighting techniques

For this most part this will just be a study project for me to chip away at. Not really planning on using it to make an actual game.

## Current Features
  - Full hardware rendering using Vulkan
  - SDL2 for cross-platform input and windowing
  - uhhh not much else

## Planned Features
  - .obj loading (just needs to be ported from the software engine)
  - Textures
  - Advanced lighting techniques
  - BSP maps made with TrenchBroom???

# Building
If you want to build this thing, you need these first:

  - [Vulkan SDK from LunarG](https://vulkan.lunarg.com/sdk/home) and follow install instructions
  - SDL2 (can be obtained through package manager)
  - Clang (GCC should be fine too, just edit the Makefile)
  - Make

Clone this repo. Then:

`cd /path/to/vulkan3D/`

`make setup shaders`

`make debug` or `make release`

# Platform Support
**Linux** should work fine.

**MacOS** might work but I need to edit the Makefile...

**Windows** is not supported.



