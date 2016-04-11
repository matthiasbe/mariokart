/* 
 * File:   SteringWheel.h
 * Author: bouvattj
 *
 * Created on 11 avril 2016, 16:22
 */

#ifndef MOVINGWHEEL_H
#define	MOVINGWHEEL_H

#include "MeshRenderable.hpp"
#include "dynamics/Particle.hpp"


class MovingWheel : public MeshRenderable{
public:
    MovingWheel(ShaderProgramPtr program, const std::string& filename, bool rotation,ParticlePtr kart);
    virtual ~MovingWheel();
private:
    void animate(float time);
    float m_angle;
    bool m_direction;
    float m_wheelDirection;
    ParticlePtr m_kart;
    void do_keyPressedEvent(sf::Event& e);
    void do_keyReleasedEvent(sf::Event& e);
};
typedef std::shared_ptr<MovingWheel> MovingWheelPtr;

#endif	/* STERINGWHEEL_H */

