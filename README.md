# Game Component Integration - Sprint 2

Title: Multiverse Cursed Village

This solution was created using Visual Studio Community 2022 Version 17 Toolset v143.

The project is currently configured to be build in Debug and Release mode, both on x64 architecture.

Currently you can only run this project through Visual Studio Solution.

The user inputs expected when using the application are:
- W moves the Player foward
- S moves the Player backward
- D rotates the Player to the right
- A rotates the Player to the left

The Texture Content Pipeline requires five programs to be defined on the System Environment Variables to run properly.
These are:
- Phyton
- Magick
- Pngquant
- Optipng
- PVRTexToolCLI
The Script that runs the content pipeline can be found on the folder ./dev/tools/texture-pipeline.py
It's defined to be run as a Post-Build Event of the Project "RandomGame"