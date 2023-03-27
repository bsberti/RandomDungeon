# Game Component Integration - Sprint 3

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

Demo Client/Server using Thrift

Includes: 
	-> Boost 1.59 x86 and x64 libs, headers, and installers
	-> OpenSSL x86 and x64 installers
	-> Thrift 0.9.1 headers and libs complied with the above
	-> Thrift 0.9.1 compiler

The included libraries have been packaged and linked directly with the Visual Studio solution. This is not recommended, as both
Boost and Thrift are so large. In general, it is expected that other developers working on the same project would have both of 
these libraries installed on their own machine already. Installation instructions follow:

Boost and OpenSSL:
	-> I have included installater files for each of these, at "$(SolutionDir)Boost\installers" and "$(SolutionDir)OpenSSL"
	-> Both should be installed to the default locations.
	-> If installing both x64 and x86, OpenSSL will install to two separate folders, and Boost will install both in the same folder

Thrift:
	-> Create a folder underneath your root drive called "Thrift" (e.g. "C:\Thrift")
	-> Copy the contents of "$(SolutionDir)Thrift" in to that folder

LINKING:
Boost:
	-> Include Directory - All Configurations - All Platforms: "C:\local\boost_1_59_0"
	-> Library Directory - All Configurations - x32: "C:\local\boost_1_59_0\lib32-msvc-14.0"
	-> Library Directory - All Configurations - x64: "C:\local\boost_1_59_0\lib64-msvc-14.0"
	-> Linker Input - Debug - All Platforms: "libboost_thread-vc140-mt-gd-1_59.lib"
	-> Linker Input - Release - All Platforms: "libboost_thread-vc140-mt-1_59.lib"

Thrift:
	-> Include Directory - All Configurations - All Platforms: "C:\Thrift\thrift-0.9.1\src"
	-> Library Directory = All Configurations - All Platforms: "C:\Thrift\thrift-0.9.1\lib\$(IntDir)"
	-> Linker Input - All Configurations - All Platforms: "libthrift.lib"