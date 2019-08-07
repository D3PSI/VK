# VK
## Vulkan part of my Vulkan vs. OpenGL comparison

This project features a big part of my matura thesis which is Comparing the Vulkan API and OpenGL with one another. This repository contains the source code for the Vulkan application.

## Installing

To install this application, simply execute the following commands:

    git clone https://github.com/D3PSI/VK.git
    cd VK/install/
    sudo ./INSTALL.sh
    cd ..
    make

This will give you the executable in `bin/Linux/x64/VK by D3PSI`. 
To run it execute the following (in the root directory of the project):

    ./RUN.sh

or

    make run

Either one should work. If it gives you errors about includes from Windows or whatever, make sure to open `VK/Version.hpp` in a text editor and check that the line containing `#define LINUX` is uncommented and that the lines containing `#define WIN_64 `, `#define WIN_32`, `#define MACOSX` are commented out. Then try to recompile the project by `cd`-ing into the projects root directory and running `make run`.

#### Hint
(If you are a little lazy like me, there are precompiled versions for Windows x64 and Debian x64 hidden somewhere in this repository. I am sure that you will manage to find them.)

## Uninstalling

Well, if you figured out how to install the project, you will manage to uninstall it on your own. It does not create any temporary files outside of the project folder, so, just delete that:

    rm -r VK/

Thank you for your interest in this project!
