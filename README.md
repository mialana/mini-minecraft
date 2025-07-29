# Mini Minecraft

![img](thumbnail.png)

**[Demo Video](https://youtu.be/KJk70-qdE_o)**

### Overview

A simple C++ program to emulate the 3D entity interactivity and world exploration themes of Minecraft.

### Build Instructions

The easiest way to get this program running is to use an IDE like QT Creator and select the project root file at `miniMinecraft/miniMinecraft.pro`.

Otherwise, use your favorite build system tool. The following configurations would be neccessary.

- Include libraries within `miniMinecraft/include`

```cmake
// example for CMake
include_directories(miniMinecraft/include)
``` 

- Install the contents of `miniMinecraft/data`, `miniMinecraft/forms`, `miniMinecraft/textures`, `miniMinecraft/glsl` to the project.

```cmake
// example for CMake

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data/")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/forms/")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/textures/")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/glsl/")
```