/* 
 * File:   SteringWheel.h
 * Author: bouvattj
 *
 * Created on 11 avril 2016, 16:22
 */

#ifndef STERINGWHEEL_H
#define	STERINGWHEEL_H

#include "MeshRenderable.hpp"


class Wheel : public MeshRenderable{
public:
    Wheel(ShaderProgramPtr program, const std::string& filename);
    virtual ~Wheel();
private:
    void animate();
};
typedef std::shared_ptr<Wheel> WheelPtr;

#endif	/* STERINGWHEEL_H */

