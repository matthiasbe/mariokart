/* 
 * File:   SteringWheel.h
 * Author: bouvattj
 *
 * Created on 11 avril 2016, 16:22
 */

#ifndef WHEEL_H
#define	WHEEL_H

#include "texturing/TexturedLightedMeshRenderable.hpp"
#include "dynamics/Particle.hpp"


class Wheel : public TexturedLightedMeshRenderable{
public:
    Wheel(ShaderProgramPtr program, const std::string& filename,
          const std::string& texture_filename , bool rotation,ParticlePtr kart);
    virtual ~Wheel();
private:
    void animate(float time);
    float angle;
    bool m_direction;
    ParticlePtr m_kart;
};
typedef std::shared_ptr<Wheel> WheelPtr;

#endif	/* STERINGWHEEL_H */

