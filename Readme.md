# Prepare Win32 Project

1. Convert it to C++17
2. Change stdafx to pch precompiled header. This is not required, but it will save you time because all default Visual Studio generated files use pch.h.


# Run Converter
1. converter input.vcxproj output.vcxproj

This creates a WinUI3 projects from an existing project.