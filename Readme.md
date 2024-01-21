# Win32 to WinUI

This repository contains a simple Win32 Project ("Win32Project" folder) and a "Converter" tool which will create a Win32ProjectWUI.vcxproj in the same folder, along with extra stuff needed for WinUI3.

# Prepare Win32 Project

1. Convert it to C++17
2. Change stdafx to pch precompiled header. This is not required, but it will save you time because all default Visual Studio generated files use pch.h.


# Run Converter
1. converter input.vcxproj output.vcxproj

This creates a WinUI3 projects from an existing project.

