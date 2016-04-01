/* 
 * File:   Kart.hpp
 * Author: teughelr
 *
 * Created on 1 avril 2016, 12:07
 */

#ifndef KART_HPP
#define	KART_HPP

#include <iostream>
#include <memory>
#include <glm/glm.hpp>

class Kart {
public:

    Kart(
            const glm::vec3& position,
            const glm::vec3& velocity,
            const glm::vec3& direction);
    ~Kart();
    
    const glm::vec3& getPosition() const;
    
    const glm::vec3& getVelocity() const;
    
    const glm::vec3& getForce() const;
    
    const glm::vec3& getDirection() const;
    
    void setPosition(const glm::vec3 &pos);
    
    void setVelocity(const glm::vec3 &vel);
    
    void restart();
    
private:
    
    const glm::vec3 m_initialPosition;
    
    const glm::vec3 m_initialVelocity;
    
    glm::vec3 m_position;
    
    glm::vec3 m_velocity;
    
    glm::vec3 m_direction;

};

typedef std::shared_ptr<Kart> KartPtr;

#endif	/* KART_HPP */

