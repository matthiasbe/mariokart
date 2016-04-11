/* 
 * File:   SteringWheel.cpp
 * Author: bouvattj
 * 
 * Created on 11 avril 2016, 16:22
 */

#include "../include/Wheel.h"
#include "../include/GeometricTransformation.hpp"

Wheel::Wheel(ShaderProgramPtr program, const std::string& filename) : 
    MeshRenderable(program,filename){
}

Wheel::~Wheel() {
}

