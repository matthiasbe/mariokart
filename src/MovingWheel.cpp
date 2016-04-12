/* 
 * File:   SteringWheel.cpp
 * Author: bouvattj
 * 
 * Created on 11 avril 2016, 16:22
 */

#include "../include/movingWheel.h"
#include "../include/GeometricTransformation.hpp"

MovingWheel::MovingWheel(ShaderProgramPtr program, const std::string& filename,bool rotation, ParticlePtr kart) : 
    MeshRenderable(program,filename){
    m_direction = rotation;
    m_kart = kart;
    m_wheelDirection = 0.f;
}

MovingWheel::~MovingWheel() {
}

void MovingWheel::animate(float time){
    float augment = glm::length(m_kart->getVelocity());
    if(!m_direction){
        m_angle -= augment*0.002;
    }else{
        m_angle += augment*0.002;
    }
    setLocalTransform(GeometricTransformation(glm::vec3{0, 0, 0},glm::quat(glm::vec3(m_angle,m_wheelDirection,0)),glm::vec3{1, 1, 1}).toMatrix());
}

void MovingWheel::do_keyPressedEvent(sf::Event& e){
    if( e.key.code == sf::Keyboard::Left ){
            m_wheelDirection = 0.6;
    }
        if( e.key.code == sf::Keyboard::Right ){
            m_wheelDirection = -0.6;
    }
    setLocalTransform(GeometricTransformation(
    glm::vec3{0, 0, 0},
    glm::quat(glm::vec3(m_angle,m_wheelDirection,0)),
    glm::vec3{1, 1, 1}).toMatrix());
}

void MovingWheel::do_keyReleasedEvent(sf::Event& e){
    if( e.key.code == sf::Keyboard::Left ){
            m_wheelDirection = 0;
    }
    if( e.key.code == sf::Keyboard::Right ){
            m_wheelDirection = 0;
    }
    setLocalTransform(GeometricTransformation(
    glm::vec3{0, 0, 0},
    glm::quat(glm::vec3(m_angle,m_wheelDirection,0)),
    glm::vec3{1, 1, 1}).toMatrix());
}