/* 
 * File:   SteringWheel.cpp
 * Author: bouvattj
 * 
 * Created on 11 avril 2016, 16:22
 */

#include <stdbool.h>

#include "../include/Wheel.h"
#include "../include/GeometricTransformation.hpp"

Wheel::Wheel(ShaderProgramPtr program, const std::string& filename,bool rotation, ParticlePtr kart) : 
    MeshRenderable(program,filename){
    m_direction = rotation;
    m_kart = kart;
}

Wheel::~Wheel() {
}

void Wheel::animate(float time){
    float augment = glm::length(m_kart->getVelocity());
    if(!m_direction){
        angle -= augment*0.002;
    }else{
        angle += augment*0.002;
    }
    setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(glm::vec3(angle,0,0)),glm::vec3{1, 1, 1}).toMatrix());
}

