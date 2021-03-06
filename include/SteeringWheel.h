/* 
 * File:   SteringWheel.h
 * Author: bouvattj
 *
 * Created on 11 avril 2016, 16:22
 */

#ifndef STERINGWHEEL_H
#define	STERINGWHEEL_H

#include "texturing/TexturedLightedMeshRenderable.hpp"


class SteringWheel : public TexturedLightedMeshRenderable{
public:
    SteringWheel(ShaderProgramPtr program, const std::string& filename, const std::string& texture_filename);
    virtual ~SteringWheel();
private:
    void animate();
    void do_keyPressedEvent(sf::Event& e);
    void do_keyReleasedEvent(sf::Event& e);
};
typedef std::shared_ptr<SteringWheel> SteringWheelPtr;

#endif	/* STERINGWHEEL_H */

