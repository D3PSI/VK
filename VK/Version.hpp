/**
    Sets build-options by defining macros

    @author        D3PSI
    @version    0.0.1 02.12.2019

    @file        Version.hpp
    @brief        Set build-options by defining macros
*/
#pragma once

// Edit configuration here
//#define VK_DEVELOPMENT      // enable this for verbose output
//#define VK_RELEASE          // enable this for performance

#define VK_WINDOW_MODE_WINDOWED
//#define VK_WINDOW_MODE_FULLSCREEN
//#define VK_WINDOW_MODE_BORDERLESS

#define WIN_64                // Windows 64-bit
//#define WIN_32              // Windows 32-bit
//#define MACOSX              // macOS X 64-bit
//#define LINUX               // Linux 64-bit

// Default values
#if !defined VK_DEVELOPMENT && !defined VK_RELEASE
    #define VK_DEVELOPMENT
#endif

#if !defined VK_WINDOW_MODE_WINDOWED && !defined VK_WINDOW_MODE_FULLSCREEN && !defined VK_WINDOW_MODE_BORDERLESS
    #define VK_WINDOW_MODE_UNDEFINED
#endif

#if !defined WIN_64 && !defined WIN_32 && !defined MACOSX && !defined LINUX
    #define WIN_64
#endif

#if !defined VK_DEVELOPMENT && defined VK_RELEASE && (defined WIN_64 || defined WIN_32)
    #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif
