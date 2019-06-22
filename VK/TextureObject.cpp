/**
    Defines the TextureObject struct

    @author       D3PSI
    @version      0.0.1 02.12.2019

    @file         TextureObject.cpp
    @brief        Definition of the TextureObject struct
*/
#pragma once
#include <string>

#include "TextureImage.hpp"
#include "TEXTURE_TYPE.cpp"


struct TextureObject {

    TextureImage*       img;
    TEXTURE_TYPE        type;
    const char*         path;

};