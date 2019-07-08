/**
    Defines the Descriptor wrapper

    @author       D3PSI
    @version      0.0.1 02.12.2019

    @file         Descriptor.hpp
    @brief        Definition of the Descriptor wrapper
*/
#pragma once
#include "UniformInfo.cpp"

class Descriptor
{
public:

    UniformInfo* info;

    /**
        Constructor

        @param      info_       A pointer to a UniformInfo structure
    */
    Descriptor(UniformInfo* info_);

};

