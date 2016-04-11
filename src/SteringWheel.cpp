/* 
 * File:   SteringWheel.cpp
 * Author: bouvattj
 * 
 * Created on 11 avril 2016, 16:22
 */

#include "../include/SteeringWheel.h"
#include "../include/GeometricTransformation.hpp"

SteringWheel::SteringWheel(ShaderProgramPtr program, const std::string& filename) : 
    MeshRenderable(program,filename){
}

SteringWheel::~SteringWheel() {
}

void SteringWheel::do_keyPressedEvent(sf::Event& e){
    if( e.key.code == sf::Keyboard::Left ){
        setLocalTransform(GeometricTransformation(
            glm::vec3{0, 0, 0},
            glm::quat(glm::vec3(0,0,1.0f)),
            glm::vec3{1, 1, 1}).toMatrix());
    }
    printf("lol");
}

void SteringWheel::do_keyReleasedEvent(sf::Event& e){

}
